#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);

  // what is an address space?
  LLVMTypeRef nyans[] = { LLVMPointerType(LLVMInt8Type(), 0) };
  LLVMTypeRef rettype = LLVMFunctionType(LLVMInt32Type(), nyans, 1, 0);
  LLVMValueRef puts_fn = LLVMAddFunction(mod, "puts", rettype);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");

  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMValueRef range[2];
  range[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  range[1] = LLVMConstInt(LLVMInt32Type(), 0, 0);

  LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 6), "simple_value");
  LLVMValueRef simple_value_pointer = LLVMBuildInBoundsGEP(builder, GlobalVar, range, 2, "simple_value");
  LLVMSetInitializer(GlobalVar, LLVMConstString("nya-n", 6, 1));
  LLVMBuildCall(builder, puts_fn, &simple_value_pointer, 1, "puts");
  
  LLVMBuildRet(builder, LLVMConstInt(LLVMInt32Type(), 31, 0));

  char *error = NULL;
  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

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

  LLVMGenericValueRef exec_args[] = { LLVMCreateGenericValueOfInt(LLVMInt32Type(), 1, 0)};
  LLVMRunFunction(engine, main, 1, exec_args);

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
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
