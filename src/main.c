//
// Created by KiberPerdun on 11/15/25.
//

#include "MIPS32.h"
#include "types.h"

i32
main (u0)
{
  MIPS32_t *cpu = MIPS32_init (NULL);
  MIPS32_free (cpu);

  return 0;
}
