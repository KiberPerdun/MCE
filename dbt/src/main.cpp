#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <filesystem>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/CodeGen.h> 
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/Support/raw_ostream.h>

#include "contract.h"
#include "mips_jit.h"

namespace fs = std::filesystem;
using namespace llvm;
using namespace llvm::orc;

typedef void (*JITFunc)(MIPS_CPU*);

extern "C" void simulate_pipeline(const TraceEntry *trace, size_t count) {
  std::printf("\n--- PIPELINE EXECUTION (PC: 0x%04x) ---\n", trace[0].pc);
  for (size_t i = 0; i < count; ++i) {
      std::printf("  [0x%04x] ID: Read($%d,$%d) -> EX -> WB($%d)\n",
                   trace[i].pc, (int)trace[i].reg_src1, (int)trace[i].reg_src2, (int)trace[i].reg_dst);
    }
}

int main(int argc, char *argv[]) {
  InitLLVM X(argc, argv);
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  fs::create_directory("jit_dumps");

  ExitOnError ExitOnErr;

  auto JTMB = ExitOnErr(JITTargetMachineBuilder::detectHost());
  JTMB.setCodeGenOptLevel(CodeGenOptLevel::Aggressive);

  auto JIT = ExitOnErr(LLJITBuilder()
                            .setJITTargetMachineBuilder(std::move(JTMB))
                            .create());

  // Сохранение дампов
  auto& ObjLayer = static_cast<ObjectLinkingLayer&>(JIT->getObjLinkingLayer());
  ObjLayer.setReturnObjectBuffer([](std::unique_ptr<MemoryBuffer> Obj) -> Expected<std::unique_ptr<MemoryBuffer>> {
    static int b_id = 0;
    std::string name = "jit_dumps/block_" + std::to_string(b_id++) + ".o";
    std::error_code EC;
    raw_fd_ostream OS(name, EC);
    if (!EC) OS << Obj->getBuffer();

    raw_fd_ostream FinalOS("jit_output.o", EC);
    if (!EC) FinalOS << Obj->getBuffer();
    return std::move(Obj);
  });

  auto &MainJD = JIT->getMainJITDylib();
  MangleAndInterner Mangle(JIT->getExecutionSession(), JIT->getDataLayout());
  ExitOnErr(MainJD.define(absoluteSymbols({{ Mangle("simulate_pipeline"), { ExecutorAddr::fromPtr(&simulate_pipeline), JITSymbolFlags::Exported } }})));
  MainJD.addGenerator(ExitOnErr(DynamicLibrarySearchGenerator::GetForCurrentProcess(JIT->getDataLayout().getGlobalPrefix())));

  MIPSTranslator translator(ThreadSafeContext(std::make_unique<LLVMContext>()), false);

  std::vector<u8> mem (1024 * 1024, 0);
  std::vector<u32> fib = {
    0x2008000A, // 1000: ADDI $8, $0, 10 (n = 10)
    0x20090000, // 1004: ADDI $9, $0, 0  (a = 0)
    0x200A0001, // 1008: ADDI $10, $0, 1 (b = 1)
    0x012A5820, // 100C: ADD $11, $9, $10 (temp = a + b)  <-- LOOP START
    0x01404820, // 1010: ADD $9, $10, $0  (a = b)
    0x01605020, // 1014: ADD $10, $11, $0 (b = temp)
    0x2108FFFF, // 1018: ADDI $8, $8, -1  (n--)
    0x1500FFFB  // 101C: BNE $8, $0, -5   (if n!=0 goto 100C)
  };

  std::vector<u32> program = {
    // --- Инициализация данных (Hi MIPS!) ---
    0x3C024869, // 1000: lui    v0,0x4869
    0x2442204D, // 1004: addiu  v0,v0,8269    (0x204D)
    0xAC020100, // 1008: sw     v0,256(zero)  (mem[256] = 0x4869204D)
    0x3C024950, // 100C: lui    v0,0x4950
    0x24425321, // 1010: addiu  v0,v0,21281   (0x5321)
    0xAC020104, // 1014: sw     v0,260(zero)  (mem[260] = 0x49505321)

    // --- Инициализация ключа ---
    0x3C020102, // 1018: lui    v0,0x102
    0x24420304, // 101C: addiu  v0,v0,772
    0xAC020108, // 1020: sw     v0,264(zero)
    0x3C020506, // 1024: lui    v0,0x506
    0x24420708, // 1028: addiu  v0,v0,1800
    0xAC02010C, // 102C: sw     v0,268(zero)
    0x3C02090A, // 1030: lui    v0,0x90a
    0x24420B0C, // 1034: addiu  v0,v0,2828
    0xAC020110, // 1038: sw     v0,272(zero)
    0x3C020D0E, // 103C: lui    v0,0xd0e
    0x24420F00, // 1040: addiu  v0,v0,3840
    0xAC020114, // 1044: sw     v0,276(zero)

    // --- Загрузка переменных в регистры ---
    0x8C040100, // 1048: lw     a0,256(zero)  (v0_tea)
    0x3C089E37, // 104C: lui    t0,0x9e37     (delta_high)
    0x8C030104, // 1050: lw     v1,260(zero)  (v1_tea)
    0x3C06C6EF, // 1054: lui    a2,0xc6ef     (target_sum_high)
    0x8C0C0108, // 1058: lw     t4,264(zero)  (k0)
    0x8C0B010C, // 105C: lw     t3,268(zero)  (k1)
    0x8C0A0110, // 1060: lw     t2,272(zero)  (k2)
    0x8C090114, // 1064: lw     t1,276(zero)  (k3)

    // --- Подготовка цикла ---
    0x00002821, // 1068: move   a1,zero       (sum = 0)
    0x250879B9, // 106C: addiu  t0,t0,31161   (delta = 0x9E3779B9)
    0x24C63720, // 1070: addiu  a2,a2,14112   (target_sum = 0xC6EF3720)

    // --- ОСНОВНОЙ ЦИКЛ (32 ИТЕРАЦИИ) ---
    0x00031100, // 1074: sll    v0,v1,0x4
    0x00033942, // 1078: srl    a3,v1,0x5
    0x00A82821, // 107C: addu   a1,a1,t0      (sum += delta)
    0x00EB3821, // 1080: addu   a3,a3,t3
    0x004C1021, // 1084: addu   v0,v0,t4
    0x00471026, // 1088: xor    v0,v0,a3
    0x00A33821, // 108C: addu   a3,a1,v1
    0x00471026, // 1090: xor    v0,v0,a3
    0x00822021, // 1094: addu   a0,a0,v0      (v0_tea += ...)
    0x00041100, // 1098: sll    v0,a0,0x4
    0x00043942, // 109C: srl    a3,a0,0x5
    0x00E93821, // 10A0: addu   a3,a3,t1
    0x004A1021, // 10A4: addu   v0,v0,t2
    0x00471026, // 10A8: xor    v0,v0,a3
    0x00A43821, // 10AC: addu   a3,a1,a0
    0x00471026, // 10B0: xor    v0,v0,a3
    0x00621821, // 10B4: addu   v1,v1,v0      (v1_tea += ...)
    0x14A6FFEE, // 10B8: bne    a1,a2,-18     (if sum != target_sum goto 1074)
    0x00000000, // 10BC: nop

    // --- СОХРАНЕНИЕ РЕЗУЛЬТАТА ---
    0xAC040100, // 10C0: sw     a0,256(zero)  (save data[0])
    0xAC030104, // 10C4: sw     v1,260(zero)  (save data[1])
    0x03E00008, // 10C8: jr     ra
    0x00000000  // 10CC: nop
  };

  MIPS_CPU cpu = {0};
  cpu.pc = 0x1000;
  cpu.memory = mem.data();
  cpu.regs[29] = 4096;
  cpu.regs[31] = 0xDEAD;
  std::map<u32, JITFunc> code_cache;

  int safety = 1000;
  while (cpu.pc >= 0x1000 && cpu.pc < 0x1000 + (program.size() * 4) && safety--) {
      std::string f_name = llvm::formatv("jit_block_{0:x}", cpu.pc).str();

      if (code_cache.find(cpu.pc) == code_cache.end()) {
          u32 idx = (cpu.pc - 0x1000) / 4;
          std::vector<u32> b_data(program.begin() + idx, program.end());
          auto TSM = translator.translateBlock(b_data, cpu.pc);
          ExitOnErr(JIT->addIRModule(std::move(TSM)));
          auto Sym = ExitOnErr(JIT->lookup(f_name));
          code_cache[cpu.pc] = reinterpret_cast<JITFunc>(Sym.getValue());
          std::printf("[JIT] Compiled optimized block: %s\n", f_name.c_str());
        }

      code_cache[cpu.pc](&cpu);
    }

  std::printf("\n--- CRYPTO EMULATION COMPLETE ---\n");
  std::printf("Program Counter (PC) = 0x%X (Should be 0xDEAD)\n", cpu.pc);
  std::printf("Cycles executed      = %d\n", 10000 - safety);

  uint32_t enc0 = *(uint32_t*)&mem[256];
  uint32_t enc1 = *(uint32_t*)&mem[260];

  std::printf("\n[+] TEA Encryption Result:\n");
  std::printf("Original (Hex) : 0x4869204D 0x49505321 (Hi MIPS!)\n");
  std::printf("Encrypted (Hex): 0x%08X 0x%08X\n", enc0, enc1);

  return 0;
}
