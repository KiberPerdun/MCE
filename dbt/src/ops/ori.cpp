//
// Created by KiberPerdun on 01.03.2026.
//

#include "mips_jit.h"

using namespace llvm;

void
MIPSTranslator::build_ORI (Value *cpu_ptr, u32 rs, u32 rt, uint16_t imm)
{
  Value *v_rs = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
  Value *v_imm = builder.getInt32 (static_cast<u32> (imm));

  Value *res = builder.CreateOr (v_rs, v_imm, "ori_tmp");

  if (rt != 0)
    builder.CreateStore (res, getRegPtr (cpu_ptr, rt));

}
