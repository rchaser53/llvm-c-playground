#include <llvm-c/Core.h>

LLVMValueRef codegen_string(LLVMModuleRef module, LLVMContextRef context, const char* str, size_t len);
