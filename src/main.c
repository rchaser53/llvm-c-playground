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
  /* setup */
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");
  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
  LlvmStruct ls = *createLlvm(mod, context, builder);
  /**/

  /**/
	LLVMTypeRef named = LLVMStructCreateNamed(context, "test_struct");
  LLVMTypeRef elements[2];
  elements[0] = LLVMInt32Type();
  elements[1] = LLVMInt32Type();
  LLVMStructSetBody(named, elements, 2, 0);

  LLVMTypeRef closure_param_types[] = { LLVMPointerType(named, 0), LLVMInt32Type() };
  LLVMTypeRef func_type = LLVMFunctionType(LLVMInt32Type(), closure_param_types, 2, 0);
  LLVMValueRef closure = LLVMAddFunction(mod, "closure", func_type);
  LLVMBasicBlockRef closure_entry = LLVMAppendBasicBlock(closure, "entry");
  LLVMPositionBuilderAtEnd(builder, closure_entry);

  LLVMValueRef target_struct = LLVMGetParam(closure, 0);
  set_field_value(builder, target_struct, 0, LLVMConstInt(LLVMInt32Type(), 12, 0));
  LLVMValueRef field = get_field_value(builder, target_struct, 0);

  LLVMValueRef closure_ret = LLVMBuildAdd(builder, field, LLVMGetParam(closure, 1), "");
  LLVMBuildRet(builder, closure_ret);
  /**/

  /**/
  LLVMTypeRef param_types[] = { LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);
  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(main, "entry");
  
  LLVMBasicBlockRef loop_block = LLVMAppendBasicBlockInContext(context, main, "loop");
  LLVMBasicBlockRef loop_end_block = LLVMAppendBasicBlockInContext(context, main, "loop_end");

  LLVMPositionBuilderAtEnd(ls.builder, entry);
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

  // char * ab = LLVMPrintModuleToString(mod);

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

LLVMValueRef get_field_value(LLVMBuilderRef builder, LLVMValueRef target_struct, int target_index)
{
  LLVMValueRef range[2];
  range[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  range[1] = LLVMConstInt(LLVMInt32Type(), target_index, 0);

  LLVMValueRef field = LLVMBuildInBoundsGEP(builder, target_struct, range, 2, "");
  return LLVMBuildLoad(builder, field, "");
}

void set_field_value(LLVMBuilderRef builder, LLVMValueRef target_struct, int target_index, LLVMValueRef value) {
  LLVMValueRef range[2];
  range[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  range[1] = LLVMConstInt(LLVMInt32Type(), target_index, 0);

  LLVMValueRef field = LLVMBuildInBoundsGEP(builder, target_struct, range, 2, "");
  LLVMBuildStore(builder, value, field);
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
