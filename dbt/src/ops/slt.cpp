//
// Created by KiberPerdun on 3/4/26.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_SLT(Value* cpu_ptr, u32 rs, u32 rt, u32 rd) {
  Value* v_rs = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
  Value* v_rt = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rt));

  Value* cmp = builder.CreateICmpSLT(v_rs, v_rt);

  Value* res = builder.CreateZExt(cmp, i32_ty);
  builder.CreateStore(res, getRegPtr(cpu_ptr, rd));
}