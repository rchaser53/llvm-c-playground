#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Transforms/Scalar.h>

#include <stdio.h>

#include "./header/util.h"

LLVMValueRef codegen_string(LLVMModuleRef module, LLVMContextRef context, const char* str)
{
  int length = sizeof(str) - 1;
  LLVMValueRef str_val = LLVMConstStringInContext(context, str, length, 0);
  LLVMValueRef g_str = LLVMAddGlobal(module, LLVMTypeOf(str_val), "");
  LLVMSetLinkage(g_str, LLVMPrivateLinkage);
  LLVMSetInitializer(g_str, str_val);
  LLVMSetGlobalConstant(g_str, 1);
  LLVMSetUnnamedAddr(g_str, 1);

  LLVMValueRef args[2];
  args[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  args[1] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  return LLVMConstInBoundsGEP(g_str, args, 2);
}

void emit_file(LLVMModuleRef module, const char * filename)
{
  char *errorMessage = NULL;
  if(LLVMPrintModuleToFile(module, filename, &errorMessage) ) {
    fprintf(stderr, "%s\n", errorMessage);
    LLVMDisposeMessage(errorMessage);
  }
}
