
#include "./header/util.h"

LLVMValueRef is_equal_int(LlvmStruct ls, int lhs_val, int rhs_val) {
  LLVMValueRef LHS = LLVMConstInt(LLVMInt32Type(), lhs_val, 0);
  LLVMValueRef RHS = LLVMConstInt(LLVMInt32Type(), rhs_val, 0);
  return LLVMBuildICmp(ls.builder, LLVMIntEQ, LHS, RHS, "");
}