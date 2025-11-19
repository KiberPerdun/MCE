//
// Created by KiberPerdun on 11/19/25.
//

#ifndef MCE_DEBUG_H
#define MCE_DEBUG_H

#ifdef DEBUG_MODE
#include <stdio.h>
  #define DEBUG_PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
  #define DEBUG_OPCODE(name) printf("Выполнение опкода: %s (0x%02X) в позиции 0x%04X\n", name, cpu->memory[cpu->pc], cpu->pc)
#else
  #define DEBUG_PRINT(fmt, ...) do {} while(0)
  #define DEBUG_OPCODE(name) do {} while(0)
#endif

#endif // MCE_DEBUG_H
