#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

LLVMValueRef llvmGenLocalStringVar(LLVMModuleRef mod, const char* data, int len) {
  LLVMValueRef glob = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), len), "string");

  LLVMSetLinkage(glob, LLVMInternalLinkage);
  LLVMSetGlobalConstant(glob, 1);
  LLVMSetInitializer(glob, LLVMConstString(data, len, 1));

  return glob;
}

int main(int argc, char const *argv[]) {
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
  LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
  LLVMBuildRet(builder, tmp);

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

  LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 6), "simple_value");
  LLVMSetInitializer(GlobalVar, LLVMConstString("nyan", 6, 1));
  uint64_t raw = LLVMGetGlobalValueAddress(engine, "simple_value");
  printf("%s\n", raw);

  llvmGenLocalStringVar(mod, "abc", 3);

  LLVMDumpModule(mod);

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
}
