//
// Created by KiberPerdun on 01.03.2026.
//
#include "mips_jit.h"
using namespace llvm;

void
MIPSTranslator::build_BNE (Value *cpu_ptr, u32 rs, u32 rt, int16_t offset,
                           u32 current_pc)
{
  Value *v_rs = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
  Value *v_rt = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rt));
  Value *cond = builder.CreateICmpNE (v_rs, v_rt);

  u32 target = current_pc + 4 + (offset << 2);
  u32 next = current_pc + 4;

  Value *res_pc = builder.CreateSelect (cond, builder.getInt32 (target),
                                        builder.getInt32 (next));
  builder.CreateStore (
      res_pc, builder.CreateStructGEP (cpu_ty, cpu_ptr, 1));
}