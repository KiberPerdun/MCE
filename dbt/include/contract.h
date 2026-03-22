#pragma once

#include <cstdint>
#include "types.h"

#define JIT_CACHE_BITS 16
#define JIT_CACHE_SIZE (1 << JIT_CACHE_BITS)

struct JITEntry
{
  u32 mips_pc;
  u0* native_ptr;
};

enum class OpType : int32_t
{
  ALU = 0,
  MEM_READ = 1,
  MEM_WRITE = 2,
  BRANCH = 3,
  UNKNOWN = 4
};

struct TraceEntry
{
  u32 pc;
  OpType type;
  u8 reg_src1;
  u8 reg_src2;
  u8 reg_dst;
  u8 padding;
  u32 mem_addr;
} __attribute__ ((packed));

struct MIPS_CPU
{
  u32 regs[32];
  u32 pc;
  u32 lo;
  u32 hi;
  u8 *memory;

  JITEntry* jit_table;

  TraceEntry* trace_buffer;
  uint32_t trace_idx;
  uint32_t trace_mask;
};
