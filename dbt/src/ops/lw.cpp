//
// Created by KiberPerdun on 01.03.2026.
//

#include "mips_jit.h"

using namespace llvm;

u0
MIPSTranslator::build_LW (llvm::Value *cpu_ptr, u32 rs, u32 rt, int16_t imm)
{
  Value* base_addr = builder.CreateLoad (i32_ty, getRegPtr (cpu_ptr, rs));
  Value* offset = builder.getInt32 ((i32) imm);
  Value* mips_addr = builder.CreateAdd (base_addr, offset);

  Value* mem_ptr_loc = builder.CreateStructGEP (cpu_ty, cpu_ptr, 4);
  Value* host_mem_base = builder.CreateLoad (PointerType::getUnqual (*TSCtx.getContext ()), mem_ptr_loc);
  Value* host_addr = builder.CreateInBoundsGEP (builder.getInt8Ty (), host_mem_base, {mips_addr});
  Value* loaded_word = builder.CreateLoad (i32_ty, host_addr);

  builder.CreateStore (loaded_word, getRegPtr (cpu_ptr, rt));
}
