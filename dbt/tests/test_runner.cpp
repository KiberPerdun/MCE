#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/Passes/PassBuilder.h>

#include "../include/contract.h"
#include "../include/mips_jit.h"

using namespace llvm;
using namespace llvm::orc;
typedef void (*JITFunc)(MIPS_CPU*);

extern "C" void simulate_pipeline(const TraceEntry *trace, size_t count) { }

bool run_test(const std::string& test_name, const std::string& bin_path, bool tracing_enabled,
          CodeGenOptLevel opt_level, const std::string& opt_name,
          const std::vector<uint32_t>& expected_memory, uint32_t mem_offset) {

  auto JTMB = cantFail(JITTargetMachineBuilder::detectHost());
  JTMB.setCodeGenOptLevel(opt_level); // Оптимизации бэкенда (выбор инструкций x86)

  std::string arch_name = JTMB.getTargetTriple().getArchName().str();

  auto JIT = cantFail(LLJITBuilder().setJITTargetMachineBuilder(std::move(JTMB)).create());
  auto &MainJD = JIT->getMainJITDylib();
  MangleAndInterner Mangle(JIT->getExecutionSession(), JIT->getDataLayout());
  cantFail(MainJD.define(absoluteSymbols({{ Mangle("simulate_pipeline"), { ExecutorAddr::fromPtr(&simulate_pipeline), JITSymbolFlags::Exported } }})));

  std::string safe_test_name = test_name;
  std::replace(safe_test_name.begin(), safe_test_name.end(), ' ', '_');
  std::string out_dir = "output/" + arch_name + "/" + opt_name + "/" + safe_test_name + (tracing_enabled ? "_trace" : "");
  std::filesystem::create_directories(out_dir);

  auto& ObjLayer = static_cast<ObjectLinkingLayer&>(JIT->getObjLinkingLayer());
  ObjLayer.setReturnObjectBuffer([out_dir, b_id = 0](std::unique_ptr<MemoryBuffer> Obj) mutable -> Expected<std::unique_ptr<MemoryBuffer>> {
    std::string base_name = out_dir + "/block_" + std::to_string(b_id++);
    std::string obj_path = base_name + ".o";
    std::string asm_path = base_name + ".asm";

    std::error_code EC;
    raw_fd_ostream OS(obj_path, EC);
    if (!EC) OS << Obj->getBuffer();
    OS.close();

    std::string cmd = "llvm-objdump -d -M intel " + obj_path + " > " + asm_path + " 2>/dev/null";
    int ret = std::system(cmd.c_str());
    (void)ret;

    return std::move(Obj);
  });

  std::ifstream file(bin_path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) return false;
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<u8> mem(1024 * 1024, 0);
  file.read(reinterpret_cast<char*>(&mem[4096]), size);

  MIPS_CPU cpu = {0};
  cpu.pc = 0x1000;
  cpu.memory = mem.data();
  cpu.regs[31] = 0xDEAD;
  cpu.regs[29] = 1024 * 1024 - 8;

  MIPSTranslator translator(ThreadSafeContext(std::make_unique<LLVMContext>()), tracing_enabled);
  std::map<u32, JITFunc> code_cache;

  int safety = 500000000;
  int blocks_compiled = 0;
  int blocks_executed = 0;

  auto start_time = std::chrono::high_resolution_clock::now();

  while (cpu.pc >= 0x1000 && cpu.pc < 0x1000 + size && safety > 0) {
      safety--;
      blocks_executed++;
      std::string f_name = llvm::formatv("jit_block_{0:x}", cpu.pc).str();

      if (code_cache.find(cpu.pc) == code_cache.end()) {
          blocks_compiled++;
          u32 idx = (cpu.pc - 0x1000) / 4;
          u32* mem_ptr = reinterpret_cast<u32*>(&mem[cpu.pc]);
          std::vector<u32> b_data(mem_ptr, mem_ptr + ((size / 4) - idx));

          auto TSM = translator.translateBlock(b_data, cpu.pc);

          TSM.withModuleDo([&](Module &M) {
            if (opt_level != CodeGenOptLevel::None) {
                LoopAnalysisManager LAM;
                FunctionAnalysisManager FAM;
                CGSCCAnalysisManager CGAM;
                ModuleAnalysisManager MAM;

                PassBuilder PB;
                PB.registerModuleAnalyses(MAM);
                PB.registerCGSCCAnalyses(CGAM);
                PB.registerFunctionAnalyses(FAM);
                PB.registerLoopAnalyses(LAM);
                PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

                OptimizationLevel PassOptLevel;
                switch(opt_level) {
                  case CodeGenOptLevel::Less:       PassOptLevel = OptimizationLevel::O1; break;
                  case CodeGenOptLevel::Default:    PassOptLevel = OptimizationLevel::O2; break;
                  case CodeGenOptLevel::Aggressive: PassOptLevel = OptimizationLevel::O3; break;
                  default:                          PassOptLevel = OptimizationLevel::O0; break;
                  }

                ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(PassOptLevel);
                MPM.run(M, MAM);
              }
          });
          // ------------------------------------------------------------

          cantFail(JIT->addIRModule(std::move(TSM)));
          auto Sym = cantFail(JIT->lookup(f_name));
          code_cache[cpu.pc] = reinterpret_cast<JITFunc>(Sym.getValue());
        }
      code_cache[cpu.pc](&cpu);
    }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

  if (cpu.pc != 0xDEAD) return false;

  uint32_t* result_mem = reinterpret_cast<uint32_t*>(&mem[mem_offset]);
  for (size_t i = 0; i < expected_memory.size(); ++i) {
      if (result_mem[i] != expected_memory[i]) return false;
    }

  std::cout << "  [OK] " << std::left << std::setw(13) << test_name
            << " | " << opt_name
            << " | Tr: " << (tracing_enabled ? "ON " : "OFF")
            << " | Cmp: " << std::setw(2) << blocks_compiled
            << " | Exe: " << std::setw(4) << blocks_executed
            << " | Time: " << std::setw(5) << duration << " us\n";

  return true;
}

struct OptConfig {
  CodeGenOptLevel level;
  std::string name;
};

int main(int argc, char *argv[]) {
  InitLLVM X(argc, argv);
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  std::string bin_dir = (argc >= 2) ? argv[1] : "tests_bin";

  std::vector<OptConfig> opt_levels = {
    {CodeGenOptLevel::None,       "O0"},
    {CodeGenOptLevel::Less,       "O1"},
    {CodeGenOptLevel::Default,    "O2"},
    {CodeGenOptLevel::Aggressive, "O3"}
  };

  std::vector<uint32_t> expected_sort = {10, 20, 30, 40, 50};
  std::vector<uint32_t> expected_tea = {0x59680A08, 0xC55E11D9};

  bool all_passed = true;

  for (const auto& opt : opt_levels) {
      std::cout << "\n--- JIT Optimization Level: " << opt.name << " ---\n";

      all_passed &= run_test("Bubble Sort", bin_dir + "/sort.bin", false, opt.level, opt.name, expected_sort, 256);
      all_passed &= run_test("Bubble Sort", bin_dir + "/sort.bin", true,  opt.level, opt.name, expected_sort, 256);

      all_passed &= run_test("TEA Crypto", bin_dir + "/tea.bin", false, opt.level, opt.name, expected_tea, 256);
      all_passed &= run_test("TEA Crypto", bin_dir + "/tea.bin", true,  opt.level, opt.name, expected_tea, 256);
    }

  std::cout << "\n---------------------------------------------------------------------------------\n";
  if (all_passed) {
      std::cout << "[+] ALL TESTS PASSED SUCCESSFULLY!\n";
      return 0;
    } else {
      std::cerr << "[-] SOME TESTS FAILED IN THE MATRIX!\n";
      return 1;
    }
}