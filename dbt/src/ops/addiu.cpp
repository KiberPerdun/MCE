//
// Created by KiberPerdun on 3/4/26.
//
#include "mips_jit.h"
using namespace llvm;

u0
MIPSTranslator::build_ADDIU (Value *cpu_ptr, u32 rs, u32 rt, i16 imm)
{
  Value *v_rs = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
  Value *res = builder.CreateAdd (v_rs, builder.getInt32 ((i32)imm));

  if (rt != 0)
    builder.CreateStore (res, getRegPtr (cpu_ptr, rt));

}