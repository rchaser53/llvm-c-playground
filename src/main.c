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

// LLVMValueRef is_cmp_llvm_int(LlvmStruct ls, LLVMIntPredicate cmp, int lhs_val, int rhs_val) {
//   LLVMValueRef LHS = LLVMConstInt(LLVMInt32Type(), lhs_val, 0);
//   LLVMValueRef RHS = LLVMConstInt(LLVMInt32Type(), rhs_val, 0);
//   return LLVMBuildICmp(ls.builder, cmp, LHS, RHS, "");
// }

int main(int argc, char const *argv[])
{
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBasicBlockRef loop_block = LLVMAppendBasicBlockInContext(context, main, "loop");
  LLVMBasicBlockRef loop_end_block = LLVMAppendBasicBlockInContext(context, main, "loop_end");
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

  LlvmStruct ls = *createLlvm(mod, context, builder);
  LLVMPositionBuilderAtEnd(ls.builder, entry);

  /**/
  LLVMTypeRef struct_param[] = { LLVMInt32Type() };

  LLVMTypeRef lfi_type = LLVMStructTypeInContext(context, struct_param, 1, 0);
  LLVMValueRef g_str = LLVMAddGlobal(mod, lfi_type, "nyan");
  // LLVMValueRef g_str = LLVMAddGlobalInAddressSpace(mod, lfi_type, "nyan", 0);
  // LLVMSetLinkage(g_str, LLVMPrivateLinkage);

  // LLVMValueRef lfi_object = LLVMBuildAlloca(builder, lfi_type, "");

  // LLVMValueRef field = LLVMBuildStructGEP(builder, lfi_object, 0, "");
  // LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 1, 0), field);
  /**/

  LLVMValueRef llvm_printf_int = create_printf_int(ls);

  LLVMValueRef val = LLVMBuildAlloca(builder, LLVMInt32Type(), "");
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 0, 0), val);

  LLVMBuildBr(builder, loop_block);
  LLVMPositionBuilderAtEnd(builder, loop_block);

  // loop
  LLVMValueRef load_val = LLVMBuildLoad(builder, val, "");
  LLVMValueRef lhs = LLVMBuildAdd(builder, LLVMConstInt(LLVMInt32Type(), 1, 0), load_val, "a.b");
  LLVMBuildStore(builder, lhs, val);

  LLVMValueRef printf_args[] = {
    codegen_string(mod, context, "%d\n"), LLVMBuildLoad(builder, val, "")
  };
  LLVMBuildCall(builder, llvm_printf_int, printf_args, 2, "");

  LLVMValueRef for_cond = LLVMBuildICmp(ls.builder, LLVMIntUGT, lhs, LLVMConstInt(LLVMInt32Type(), 2, 0), "");
  LLVMBuildCondBr(builder, for_cond, loop_end_block, loop_block);
  LLVMPositionBuilderAtEnd(builder, loop_end_block);

  // loop end
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
