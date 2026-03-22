//
// Created by KiberPerdun on 3/4/26.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_SLL (Value* cpu_ptr, u32 rt, u32 rd, u32 sa) {
  Value* v_rt = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rt));
  Value* res = builder.CreateShl(v_rt, builder.getInt32(sa));
  builder.CreateStore(res, getRegPtr(cpu_ptr, rd));
}
