//
// Created by KiberPerdun on 11/15/25.
//

#ifndef MCE_MIPS32_H
#define MCE_MIPS32_H

#include "types.h"
#include "stdio.h"

typedef WORD MIPS32_gen_reg;
typedef WORD MIPS32_HI_reg;
typedef WORD MIPS32_LO_reg;
typedef WORD MIPS32_PC_reg;

#define MIPS32_INSTRUCTION_SIZE 4
#define MIPS32_INSTRUCTION_NUM 64
#define MIPS32_NUM_GEN_REGS 32

typedef struct MIPS32
{
  union
  {
    struct
    {
      MIPS32_gen_reg r0; /* zero */
      MIPS32_gen_reg r1;
      MIPS32_gen_reg r2;
      MIPS32_gen_reg r3;
      MIPS32_gen_reg r4;
      MIPS32_gen_reg r5;
      MIPS32_gen_reg r6;
      MIPS32_gen_reg r7;
      MIPS32_gen_reg r8;
      MIPS32_gen_reg r9;
      MIPS32_gen_reg r10;
      MIPS32_gen_reg r11;
      MIPS32_gen_reg r12;
      MIPS32_gen_reg r13;
      MIPS32_gen_reg r14;
      MIPS32_gen_reg r15;
      MIPS32_gen_reg r16;
      MIPS32_gen_reg r17;
      MIPS32_gen_reg r18;
      MIPS32_gen_reg r19;
      MIPS32_gen_reg r20;
      MIPS32_gen_reg r21;
      MIPS32_gen_reg r22;
      MIPS32_gen_reg r23;
      MIPS32_gen_reg r24;
      MIPS32_gen_reg r25;
      MIPS32_gen_reg r26;
      MIPS32_gen_reg r27;
      MIPS32_gen_reg r28;
      MIPS32_gen_reg r29;
      MIPS32_gen_reg r30;
      MIPS32_gen_reg r31;
    };
    MIPS32_gen_reg regs[MIPS32_NUM_GEN_REGS];
  };
  MIPS32_HI_reg hi;
  MIPS32_LO_reg lo;
  MIPS32_PC_reg pc;

  union
  {
    BYTE *memory;
    BYTE *ram;
  };
} MIPS32_t;

exception MIPS32_unknown_opcode (MIPS32_t *cpu);

typedef exception (*MIPS32_opcode_handler_t)(MIPS32_t *cpu);
extern MIPS32_opcode_handler_t MIPS32_opcode_table[MIPS32_INSTRUCTION_NUM];

typedef struct MIPS32_insctruction_i_type
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  WORD opcode : 6;
  WORD rs : 5;
  WORD rt : 5;
  WORD imm : 16;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  WORD imm : 16;
  WORD rt : 5;
  WORD rs : 5;
  WORD opcode : 6;
#endif
} MIPS32_instruction_i_type_t;

typedef struct MIPS32_insctruction_j_type
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  WORD opcode : 6;
  WORD address : 26;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  WORD address : 26;
  WORD opcode : 6;
#endif
} MIPS32_instruction_j_type_t;

typedef struct MIPS32_insctruction_r_type
{
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  WORD opcode : 6;
  WORD rs : 5;
  WORD rt : 5;
  WORD rd : 5;
  WORD shamt : 5;
  WORD funct : 6;
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  WORD funct : 6;
  WORD shamt : 5;
  WORD rd : 5;
  WORD rt : 5;
  WORD rs : 5;
  WORD opcode : 6;
#endif
} MIPS32_instruction_r_type_t;

typedef struct MIPS32_insctruction
{
  union
  {
    MIPS32_instruction_i_type_t i;
    MIPS32_instruction_j_type_t j;
    MIPS32_instruction_r_type_t r;
    WORD raw;
  };
} MIPS32_insctruction_t;

MIPS32_t *MIPS32_init (MIPS32_t *cpu);
MIPS32_t *MIPS32_zero_init (MIPS32_t *cpu);
MIPS32_t *MIPS32_free (MIPS32_t *cpu);
MIPS32_t *MIPS32_run (MIPS32_t *cpu);

#endif // MCE_MIPS32_H
