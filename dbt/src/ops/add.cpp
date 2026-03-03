//
// Created by KiberPerdun on 01.03.2026.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_ADD (Value *cpu_ptr, u32 rs, u32 rt, u32 rd)
{
  Value *v_rs = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
  Value *v_rt = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rt));

  Value *res = builder.CreateAdd (v_rs, v_rt);

  if (rd != 0)
    builder.CreateStore (res, getRegPtr (cpu_ptr, rd));

}
