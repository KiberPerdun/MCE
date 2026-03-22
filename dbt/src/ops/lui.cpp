//
// Created by KiberPerdun on 01.03.2026.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_LUI (Value* cpu_ptr, u32 rt, uint16_t imm)
{
  Value* res = builder.CreateShl(builder.getInt32(imm), 16);
  builder.CreateStore(res, getRegPtr(cpu_ptr, rt));
}