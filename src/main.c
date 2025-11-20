//
// Created by KiberPerdun on 11/15/25.
//

#include "MIPS32.h"
#include "types.h"
#include <malloc.h>

i32
main (u0)
{
  MIPS32_t *cpu = MIPS32_init (NULL);

  cpu->memory = malloc (1024);
  MIPS32_instruction_i_type_t *ins;
  ins = (MIPS32_instruction_i_type_t *) cpu->memory;
  ins->opcode = 0b001000;
  ins->rs = 13;
  ins->rt = 1;
  ins->imm = -1;
  MIPS32_run (cpu);
  MIPS32_dump_state (cpu);
  free (cpu->memory);

  MIPS32_free (cpu);

  return 0;
}
