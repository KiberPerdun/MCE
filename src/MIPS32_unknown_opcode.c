//
// Created by KiberPerdun on 11/19/25.
//

#include "MIPS32.h"

exception
MIPS32_unknown_opcode (MIPS32_t *cpu)
{
  printf ("unknown opcode at 0x%04X\n", cpu->pc);
  cpu->pc += MIPS32_INSTRUCTION_SIZE;

  return 1;
}

MIPS32_opcode_handler_t MIPS32_opcode_table[MIPS32_INSTRUCTION_NUM] = { MIPS32_unknown_opcode };