//
// Created by KiberPerdun on 3/4/26.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_J (llvm::Value* cpu_ptr, u32 current_pc, u32 imm26)
{
  u32 target = ((current_pc + 4) & 0xF0000000) | (imm26 << 2);
  builder.CreateStore (builder.getInt32 (target), builder.CreateStructGEP (cpu_ty, cpu_ptr, 1));
}