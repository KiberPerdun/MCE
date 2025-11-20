//
// Created by KiberPerdun on 11/19/25.
//

#include "MIPS32.h"
#include "MIPS32_math.h"
#include <malloc.h>

MIPS32_t *
MIPS32_init (MIPS32_t *cpu)
{
  for (i16 i = 0; i < MIPS32_INSTRUCTION_NUM; ++i)
    MIPS32_opcode_table[i] = MIPS32_unknown_opcode;

  MIPS32_opcode_table[0b001000] = MIPS32_addi;

  if (cpu)
    return cpu;

  else
    return (MIPS32_t *) calloc (1, sizeof (MIPS32_t));
}
