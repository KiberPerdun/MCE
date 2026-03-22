#pragma once
#include "contract.h"
#include "types.h"
#include <cstdint>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <vector>

using u32 = uint32_t;
using i32 = int32_t;

class MIPSTranslator {
  public:
  MIPSTranslator (llvm::orc::ThreadSafeContext ctx,
                  bool enable_tracing = true);

      llvm::orc::ThreadSafeModule
      translateBlock (const std::vector<u32> &mips_code, u32 start_pc);

  private:
  llvm::orc::ThreadSafeContext TSCtx;
  llvm::IRBuilder<> builder;
  bool enable_tracing;

  llvm::Type* i32_ty;
  llvm::StructType* cpu_ty;
  llvm::StructType* trace_entry_ty;
  llvm::FunctionType* simulate_func_ty;

  void initTypes (llvm::Module* M);

  llvm::Value* getRegPtr (llvm::Value* cpu_ptr, u32 reg_idx) {
    return builder.CreateInBoundsGEP (cpu_ty, cpu_ptr,
                                      { builder.getInt32 (0),
                                        builder.getInt32 (0),
                                        builder.getInt32 (reg_idx) });
  }

  OpType decideOpType(u32 opcode, u32 funct);

  void build_ADD (llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_ADDI (llvm::Value* cpu_ptr, u32 rs, u32 rt, int16_t imm);
  void build_ORI (llvm::Value* cpu_ptr, u32 rs, u32 rt, uint16_t imm);
  void build_LW (llvm::Value *cpu_ptr, u32 rs, u32 rt, int16_t imm);
  void build_SW (llvm::Value* cpu_ptr, u32 rs, u32 rt, int16_t imm);
  void build_ADDIU (llvm::Value *cpu_ptr, u32 rs, u32 rt, i16 imm);
  void build_J (llvm::Value* cpu_ptr, u32 current_pc, u32 imm26);
  void build_JR (llvm::Value* cpu_ptr, u32 rs);
  void build_OR (llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_SLL (llvm::Value* cpu_ptr, u32 rt, u32 rd, u32 sa);
  void build_SLT (llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_SUB (llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_XOR (llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_SRL (llvm::Value* cpu_ptr, u32 rt, u32 rd, u32 sa);
  void build_LUI (llvm::Value* cpu_ptr, u32 rt, uint16_t imm);
};