//
// Created by KiberPerdun on 3/4/26.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_JR (llvm::Value* cpu_ptr, u32 rs)
{
  Value* target_addr = builder.CreateLoad(i32_ty, getRegPtr(cpu_ptr, rs));
  builder.CreateStore (target_addr, builder.CreateStructGEP (cpu_ty, cpu_ptr, 1));
}