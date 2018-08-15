#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Transforms/Scalar.h>

typedef struct llvmStruct {
  LLVMModuleRef module;
  LLVMContextRef context;
  LLVMBuilderRef builder;
} LlvmStruct;

LLVMValueRef codegen_string(LLVMModuleRef module, LLVMContextRef context, const char* str);
LlvmStruct *createLlvm(LLVMModuleRef module, LLVMContextRef context, LLVMBuilderRef builder);
LLVMValueRef create_printf_int(LlvmStruct ls);

void emit_file(LLVMModuleRef module, const char * filename);
void optimization(LLVMModuleRef mod);
