#include <llvm-c/Core.h>

LLVMValueRef codegen_string(LLVMModuleRef module, LLVMContextRef context, const char* str, size_t len);
void emit_file(LLVMModuleRef module, const char * filename);
void ho();
void optimization(LLVMModuleRef mod);
