//
// Created by KiberPerdun on 11/20/25.
//

#include "MIPS32.h"

MIPS32_t *
MIPS32_run (MIPS32_t *cpu)
{
  MIPS32_instruction_t *ins;

  for (ins = (MIPS32_instruction_t *) (cpu->memory + cpu->pc);
       !MIPS32_opcode_table[ins->i.opcode] (cpu);
       cpu->pc += MIPS32_INSTRUCTION_SIZE, ins = (MIPS32_instruction_t *) (cpu->memory + cpu->pc))
    ;

  return cpu;
}
