//
// Created by KiberPerdun on 11/19/25.
//

#include "MIPS32.h"

exception
MIPS32_unknown_opcode (MIPS32_t *cpu)
{
  BYTE byte = cpu->memory[cpu->pc];
  printf ("unknown opcode 0x%02X at 0x%04X\n", byte, cpu->pc);
  cpu->pc += MIPS32_INSTRUCTION_SIZE;

  return 1;
}

MIPS32_opcode_handler_t MIPS32_opcode_table[MIPS32_INSTRUCTION_NUM] = { MIPS32_unknown_opcode };