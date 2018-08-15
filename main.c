#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Transforms/Scalar.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "./header/util.h"

int main(int argc, char const *argv[])
{
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");

  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
  LlvmStruct ls = *createLlvm(mod, context, builder);

  LLVMPositionBuilderAtEnd(ls.builder, entry);

  LLVMValueRef llvm_printf_int = create_printf_int(ls);

  LLVMBasicBlockRef entry_block = LLVMGetInsertBlock(builder);
  LLVMBasicBlockRef left_block = LLVMAppendBasicBlockInContext(context, main, "left");
  LLVMBasicBlockRef right_block = LLVMAppendBasicBlockInContext(context, main, "right");

  LLVMValueRef val = LLVMBuildAlloca(builder, LLVMInt32Type(), "ret_val");
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 11, 0), val);

  LLVMBuildCondBr(builder, LLVMConstInt(LLVMInt1TypeInContext(context), 0, 0), left_block, right_block);

  LLVMPositionBuilderAtEnd(builder, left_block);

  LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 22, 0), val);
  LLVMBuildBr(builder, right_block);
  LLVMPositionBuilderAtEnd(builder, right_block);

  LLVMValueRef printf_args[] = {
    codegen_string(mod, context, "%d\n"), LLVMBuildLoad(builder, val, "ret_val")
  };
  LLVMBuildCall(builder, llvm_printf_int, printf_args, 2, "");
  LLVMBuildRet(builder, LLVMConstInt(LLVMInt32Type(), 0, 0));

  char *error = NULL;
  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  optimization(mod);

  LLVMExecutionEngineRef engine;
  error = NULL;
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeX86AsmPrinter();
  LLVMInitializeX86Disassembler();

  if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
    fprintf(stderr, "failed to create execution engine\n");
    abort();
  }

  if (error) {
    fprintf(stderr, "error: %s\n", error);
    LLVMDisposeMessage(error);
    exit(EXIT_FAILURE);
  }
  LLVMDumpModule(mod);

  emit_file(mod, "main.ll");

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
}

void optimization(LLVMModuleRef mod)
{
  LLVMPassManagerRef pas_manager = LLVMCreateFunctionPassManagerForModule(mod);
  LLVMAddInstructionCombiningPass(pas_manager);
  LLVMAddReassociatePass(pas_manager);
  LLVMAddGVNPass(pas_manager);
  LLVMAddCFGSimplificationPass(pas_manager);
  LLVMInitializeFunctionPassManager(pas_manager);
}

  // this remove
    /**
    * Remove a function from its containing module and deletes it.
    *
    * @see llvm::Function::eraseFromParent()
    */
    // void LLVMDeleteFunction(LLVMValueRef Fn);
  // ex.
  //  LLVMDeleteFunction(sum);

  // run function
  // LLVMGenericValueRef exec_args[] = { LLVMCreateGenericValueOfInt(LLVMInt32Type(), 1, 0)};
  // LLVMRunFunction(engine, main, 1, exec_args);
