#include "mips_jit.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FormatVariadic.h>

using namespace llvm;

MIPSTranslator::MIPSTranslator (orc::ThreadSafeContext ctx)
    : TSCtx (std::move (ctx)), builder (*TSCtx.getContext ())
{
}

void
MIPSTranslator::initTypes (Module *M)
{
  auto &ctx = *TSCtx.getContext ();
  i32_ty = Type::getInt32Ty (ctx);
  auto i8_ty = Type::getInt8Ty (ctx);

  cpu_ty = StructType::create (ctx, "MIPS_CPU");
  cpu_ty->setBody ({ ArrayType::get (i32_ty, 32), i32_ty, i32_ty, i32_ty });

  trace_entry_ty = StructType::create (ctx, "TraceEntry");
  trace_entry_ty->setBody (
      { i32_ty, i32_ty, i8_ty, i8_ty, i8_ty, i8_ty, i32_ty }, true);

  simulate_func_ty = FunctionType::get (
      builder.getVoidTy (),
      { PointerType::getUnqual (trace_entry_ty), Type::getInt64Ty (ctx) },
      false);
}

OpType
MIPSTranslator::decideOpType (u32 opcode, u32 funct)
{
  if (opcode == 0x05)
    return OpType::BRANCH;

  return OpType::ALU;
}

Expected<orc::ThreadSafeModule>
MIPSTranslator::translateBlock (const std::vector<u32> &mips_code,
                                u32 start_pc)
{
  auto &ctx = *TSCtx.getContext ();
  auto M = std::make_unique<Module> (
      llvm::formatv ("mod_{0:x}", start_pc).str (), ctx);
  initTypes (M.get ());

  std::string func_name = llvm::formatv ("jit_block_{0:x}", start_pc).str ();

  auto func_ty = FunctionType::get (
      builder.getVoidTy (), { PointerType::getUnqual (cpu_ty) }, false);
  auto func = Function::Create (func_ty, Function::ExternalLinkage, func_name,
                                M.get ());
  auto entry = BasicBlock::Create (ctx, "entry", func);
  builder.SetInsertPoint (entry);

  Value *cpu_ptr = func->arg_begin ();

  size_t count = 0;
  for (u32 inst : mips_code)
    {
      count++;
      if (decideOpType (inst >> 26, inst & 0x3F) == OpType::BRANCH)
        break;

    }

  auto trace_array = builder.CreateAlloca (
      trace_entry_ty, builder.getInt64 (count), "trace_buf");

  u32 pc = start_pc;
  for (size_t i = 0; i < count; ++i)
    {
      u32 inst = mips_code[i];
      u32 op = inst >> 26;
      u32 rs = (inst >> 21) & 0x1F;
      u32 rt = (inst >> 16) & 0x1F;
      u32 rd = (inst >> 11) & 0x1F;
      int16_t imm = (int16_t)(inst & 0xFFFF);

      if (op == 0x00 && (inst & 0x3F) == 0x20)
        build_ADD (cpu_ptr, rs, rt, rd);

      else if (op == 0x08)
        build_ADDI (cpu_ptr, rs, rt, imm);

      else if (op == 0x0D)
        build_ORI (cpu_ptr, rs, rt, (uint16_t)imm);

      else if (op == 0x05)
        {
          Value *v_rs = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
          Value *v_rt = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rt));
          Value *cond = builder.CreateICmpNE (v_rs, v_rt);

          u32 target
              = pc + 4 + (imm << 2);
          u32 next = pc + 4;

          Value *res_pc = builder.CreateSelect (
              cond, builder.getInt32 (target), builder.getInt32 (next));
          builder.CreateStore (
              res_pc, builder.CreateStructGEP (cpu_ty, cpu_ptr, 1));
        }
      else
        {
          builder.CreateStore (builder.getInt32 (pc + 4),
                               builder.CreateStructGEP (cpu_ty, cpu_ptr, 1));
        }

      Value *e_ptr = builder.CreateInBoundsGEP (trace_entry_ty, trace_array,{ builder.getInt64 (i) });
      builder.CreateStore (builder.getInt32 (pc),builder.CreateStructGEP (trace_entry_ty, e_ptr, 0));
      builder.CreateStore (builder.getInt32 ((i32)decideOpType (op, inst & 0x3F)),builder.CreateStructGEP (trace_entry_ty, e_ptr, 1));
      builder.CreateStore (builder.getInt8 (rs),builder.CreateStructGEP (trace_entry_ty, e_ptr, 2));
      builder.CreateStore (builder.getInt8 (rt),builder.CreateStructGEP (trace_entry_ty, e_ptr, 3));
      builder.CreateStore (builder.getInt8 ((op == 0) ? rd : rt),builder.CreateStructGEP (trace_entry_ty, e_ptr, 4));
      builder.CreateStore (builder.getInt32 (0),builder.CreateStructGEP (trace_entry_ty, e_ptr, 6));

      pc += 4;
    }

  auto simulate_fn
      = M->getOrInsertFunction ("simulate_pipeline", simulate_func_ty);
  builder.CreateCall (simulate_fn, { trace_array, builder.getInt64 (count) });
  builder.CreateRetVoid ();

  return orc::ThreadSafeModule (std::move (M), TSCtx);
}