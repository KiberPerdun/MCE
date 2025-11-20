//
// Created by KiberPerdun on 11/20/25.
//

#include "MIPS32_math.h"
#include <string.h>

exception
MIPS32_addi (MIPS32_t *cpu)
{
  MIPS32_instruction_i_type_t ins;

  memcpy (&ins, cpu->memory + cpu->pc, MIPS32_INSTRUCTION_SIZE);
  if (ins.rt == 0)
    return 1;

  i32 simm = (i32)(i16) ins.imm;
  i32 rs_val = (i32) cpu->regs[ins.rs];
  cpu->regs[ins.rt] = (u32) (simm + rs_val);

  return 0;
}
