#include "mips_jit.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FormatVariadic.h>

using namespace llvm;

MIPSTranslator::MIPSTranslator(orc::ThreadSafeContext ctx, bool trace)
    : TSCtx(std::move(ctx)), builder(*TSCtx.getContext()), enable_tracing(trace) {}

void MIPSTranslator::initTypes(Module *M) {
  auto &ctx = *TSCtx.getContext();
  i32_ty = Type::getInt32Ty(ctx);
  auto i8_ty = Type::getInt8Ty(ctx);
  auto ptr_ty = PointerType::getUnqual(ctx);

  cpu_ty = StructType::create(ctx, "MIPS_CPU");
  cpu_ty->setBody({ArrayType::get(i32_ty, 32), i32_ty, i32_ty, i32_ty, ptr_ty});

  trace_entry_ty = StructType::create(ctx, "TraceEntry");
  trace_entry_ty->setBody({i32_ty, i32_ty, i8_ty, i8_ty, i8_ty, i8_ty, i32_ty}, true);

  simulate_func_ty = FunctionType::get(builder.getVoidTy(), {PointerType::getUnqual(trace_entry_ty), Type::getInt64Ty(ctx)}, false);
}

OpType MIPSTranslator::decideOpType(u32 opcode, u32 funct) {
  if (opcode == 0x01 || opcode == 0x02 || opcode == 0x04 || opcode == 0x05 || opcode == 0x06 || opcode == 0x07) return OpType::BRANCH;
  if (opcode == 0x00 && funct == 0x08) return OpType::BRANCH;
  if (opcode == 0x23) return OpType::MEM_READ;
  if (opcode == 0x2B) return OpType::MEM_WRITE;
  return OpType::ALU;
}

orc::ThreadSafeModule MIPSTranslator::translateBlock(const std::vector<u32> &mips_code, u32 start_pc) {
  auto &ctx = *TSCtx.getContext();
  auto M = std::make_unique<Module>(llvm::formatv("mod_{0:x}", start_pc).str(), ctx);
  initTypes(M.get());

  std::string func_name = llvm::formatv("jit_block_{0:x}", start_pc).str();
  auto func_ty = FunctionType::get(builder.getVoidTy(), {PointerType::getUnqual(cpu_ty)}, false);
  auto func = Function::Create(func_ty, Function::ExternalLinkage, func_name, M.get());
  auto entry = BasicBlock::Create(ctx, "entry", func);
  builder.SetInsertPoint(entry);

  Value *cpu_ptr = func->arg_begin();

  size_t count = 0;
  bool delay_slot = false;
  for (u32 inst : mips_code) {
      count++;
      if (delay_slot) break;
      if (decideOpType(inst >> 26, inst & 0x3F) == OpType::BRANCH) delay_slot = true;
    }

  Value *trace_array = nullptr;
  if (enable_tracing) trace_array = builder.CreateAlloca(trace_entry_ty, builder.getInt64(count), "trace_buf");

  u32 pc = start_pc;
  Value* next_pc_val = nullptr;

  for (size_t i = 0; i < count; ++i) {
      u32 inst = mips_code[i];
      u32 op = inst >> 26;
      u32 rs = (inst >> 21) & 0x1F;
      u32 rt = (inst >> 16) & 0x1F;
      u32 rd = (inst >> 11) & 0x1F;
      int16_t imm = (int16_t)(inst & 0xFFFF);
      u32 funct = inst & 0x3F;
      u32 imm26 = inst & 0x03FFFFFF;
      u32 sa = (inst >> 6) & 0x1F;

      // Внешние функции
      if (op == 0x00 && (funct == 0x20 || funct == 0x21)) build_ADD(cpu_ptr, rs, rt, rd);
      else if (op == 0x08 || op == 0x09) build_ADDI(cpu_ptr, rs, rt, imm);
      else if (op == 0x0D) build_ORI(cpu_ptr, rs, rt, (uint16_t)imm);
      else if (op == 0x23) build_LW(cpu_ptr, rs, rt, imm);
      else if (op == 0x2B) build_SW(cpu_ptr, rs, rt, imm);
      else if (op == 0x00 && funct == 0x00) build_SLL(cpu_ptr, rt, rd, sa);
      else if (op == 0x00 && funct == 0x2A) build_SLT(cpu_ptr, rs, rt, rd);
      else if (op == 0x00 && (funct == 0x22 || funct == 0x23)) build_SUB(cpu_ptr, rs, rt, rd);
      else if (op == 0x00 && funct == 0x26) build_XOR(cpu_ptr, rs, rt, rd);
      else if (op == 0x00 && funct == 0x02) build_SRL(cpu_ptr, rt, rd, sa);
      else if (op == 0x0F) build_LUI(cpu_ptr, rt, (uint16_t)imm);

      // --- INLINE ИНСТРУКЦИИ ДЛЯ GCC ---
      else if (op == 0x0A) { // SLTI
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *cmp = builder.CreateICmpSLT(v_rs, builder.getInt32((int32_t)imm));
          builder.CreateStore(builder.CreateZExt(cmp, i32_ty), getRegPtr(cpu_ptr, rt));
        }
      else if (op == 0x0B) { // SLTIU
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *cmp = builder.CreateICmpULT(v_rs, builder.getInt32((int32_t)imm));
          builder.CreateStore(builder.CreateZExt(cmp, i32_ty), getRegPtr(cpu_ptr, rt));
        }
      else if (op == 0x0C) { // ANDI
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          builder.CreateStore(builder.CreateAnd(v_rs, builder.getInt32((uint16_t)imm)), getRegPtr(cpu_ptr, rt));
        }
      else if (op == 0x00 && funct == 0x24) { // AND
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *v_rt = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rt));
          builder.CreateStore(builder.CreateAnd(v_rs, v_rt), getRegPtr(cpu_ptr, rd));
        }
      else if (op == 0x00 && funct == 0x25) { // OR (Это используется для псевдокоманды 'move'!)
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *v_rt = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rt));
          builder.CreateStore(builder.CreateOr(v_rs, v_rt), getRegPtr(cpu_ptr, rd));
        }

      // --- ВЕТВЛЕНИЯ ---
      else if (op == 0x04 || op == 0x05) {
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *v_rt = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rt));
          Value *cond = (op == 0x04) ? builder.CreateICmpEQ(v_rs, v_rt) : builder.CreateICmpNE(v_rs, v_rt);
          next_pc_val = builder.CreateSelect(cond, builder.getInt32(pc + 4 + (imm << 2)), builder.getInt32(pc + 8));
        }
      else if (op == 0x06 || op == 0x07) {
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *zero = builder.getInt32(0);
          Value *cond = (op == 0x06) ? builder.CreateICmpSLE(v_rs, zero) : builder.CreateICmpSGT(v_rs, zero);
          next_pc_val = builder.CreateSelect(cond, builder.getInt32(pc + 4 + (imm << 2)), builder.getInt32(pc + 8));
        }
      else if (op == 0x01) {
          Value *v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
          Value *zero = builder.getInt32(0);
          Value *cond = (rt == 1) ? builder.CreateICmpSGE(v_rs, zero) : builder.CreateICmpSLT(v_rs, zero);
          next_pc_val = builder.CreateSelect(cond, builder.getInt32(pc + 4 + (imm << 2)), builder.getInt32(pc + 8));
        }
      else if (op == 0x02) {
          next_pc_val = builder.getInt32(((pc + 4) & 0xF0000000) | (imm26 << 2));
        }
      else if (op == 0x00 && funct == 0x08) {
          next_pc_val = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
        }
      // ЕСЛИ ИНСТРУКЦИЯ НЕ НАЙДЕНА - ОРЕМ В КОНСОЛЬ!
      else {
          std::printf("[!] CRITICAL JIT WARNING: Unknown instruction 0x%08X (op=%d, funct=%d) at PC: 0x%08X\n", inst, op, funct, pc);
        }

      if (enable_tracing) {
          Value *e_ptr = builder.CreateInBoundsGEP(trace_entry_ty, trace_array, {builder.getInt64(i)});
          builder.CreateStore(builder.getInt32(pc), builder.CreateStructGEP(trace_entry_ty, e_ptr, 0));
          builder.CreateStore(builder.getInt32((i32)decideOpType(op, funct)), builder.CreateStructGEP(trace_entry_ty, e_ptr, 1));
          builder.CreateStore(builder.getInt8(rs), builder.CreateStructGEP(trace_entry_ty, e_ptr, 2));
          builder.CreateStore(builder.getInt8(rt), builder.CreateStructGEP(trace_entry_ty, e_ptr, 3));
          builder.CreateStore(builder.getInt8((op == 0) ? rd : rt), builder.CreateStructGEP(trace_entry_ty, e_ptr, 4));
          builder.CreateStore(builder.getInt32(0), builder.CreateStructGEP(trace_entry_ty, e_ptr, 6));
        }

      pc += 4;
    }

  if (!next_pc_val) next_pc_val = builder.getInt32(start_pc + count * 4);
  builder.CreateStore(next_pc_val, builder.CreateStructGEP(cpu_ty, cpu_ptr, 1));

  if (enable_tracing) {
      auto simulate_fn = M->getOrInsertFunction("simulate_pipeline", simulate_func_ty);
      builder.CreateCall(simulate_fn, {trace_array, builder.getInt64(count)});
    }

  builder.CreateRetVoid();
  return orc::ThreadSafeModule(std::move(M), TSCtx);
}