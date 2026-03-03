//
// Created by KiberPerdun on 01.03.2026.
//
#pragma once
#include "contract.h"
#include "types.h"
#include <cstdint>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>
#include <memory>
#include <vector>

struct MIPS_CPU
{
  u32 regs[32];
  u32 pc;
  u32 hi;
  u32 lo;
};

class MIPSTranslator {
public:
  // Используем полные пути к типам LLVM для избежания конфликтов
  MIPSTranslator(llvm::orc::ThreadSafeContext ctx);

  llvm::Expected<llvm::orc::ThreadSafeModule> translateBlock(const std::vector<u32>& mips_code, u32 start_pc);

private:
  llvm::orc::ThreadSafeContext TSCtx;
  llvm::IRBuilder<> builder;

  llvm::Type* i32_ty;
  llvm::StructType* cpu_ty;
  llvm::StructType* trace_entry_ty;
  llvm::FunctionType* simulate_func_ty;

  void initTypes(llvm::Module* M);

  // Вспомогательный метод для получения указателя на регистр
  llvm::Value* getRegPtr(llvm::Value* cpu_ptr, u32 reg_idx) {
    return builder.CreateInBoundsGEP(cpu_ty, cpu_ptr,
        {builder.getInt32(0), builder.getInt32(0), builder.getInt32(reg_idx)});
  }

  OpType decideOpType(u32 opcode, u32 funct);

  // Операции
  void build_ADD(llvm::Value* cpu_ptr, u32 rs, u32 rt, u32 rd);
  void build_ADDI(llvm::Value* cpu_ptr, u32 rs, u32 rt, int16_t imm);
  void build_ORI(llvm::Value* cpu_ptr, u32 rs, u32 rt, uint16_t imm);
  void build_BNE(llvm::Value* cpu_ptr, u32 rs, u32 rt, int16_t offset, u32 current_pc);
};