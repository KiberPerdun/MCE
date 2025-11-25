//
// Created by KiberPerdun on 11/20/25.
//

#include "MIPS32_math.h"
#include <string.h>

exception
MIPS32_add (MIPS32_t *cpu)
{
  MIPS32_instruction_r_type_t ins;

  memcpy (&ins, cpu->memory + cpu->pc, MIPS32_INSTRUCTION_SIZE);
  if (ins.rd == 0)
    return 1;

  i32 rs_val = (i32) cpu->regs[ins.rs];
  i32 rt_val = (i32) cpu->regs[ins.rt];

  cpu->regs[ins.rd] = (u32) (rt_val + rs_val);

  return 0;
}
