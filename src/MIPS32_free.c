//
// Created by KiberPerdun on 11/19/25.
//

#include "MIPS32.h"
#include <malloc.h>

MIPS32_t *
MIPS32_free (MIPS32_t *cpu)
{
  free (cpu);
  return NULL;
}
