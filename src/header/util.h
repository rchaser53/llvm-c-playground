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
LLVMValueRef create_printf_int(LlvmStruct ls);
LLVMValueRef get_field_value(LLVMBuilderRef builder, LLVMValueRef target_struct, int target_index);
void set_field_value(LLVMBuilderRef builder, LLVMValueRef target_struct, int target_index, LLVMValueRef value);

LLVMValueRef is_equal_int(LlvmStruct ls, int lhs_val, int rhs_val);

LlvmStruct *createLlvm(LLVMModuleRef module, LLVMContextRef context, LLVMBuilderRef builder);

void emit_file(LLVMModuleRef module, const char * filename);
void optimization(LLVMModuleRef mod);
