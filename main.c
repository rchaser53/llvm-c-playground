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

  const char error_msg_str[] = "Error: couldn't start runtime!";
  LLVMValueRef honto = codegen_string(mod, context, error_msg_str, sizeof(error_msg_str) - 1);

  LLVMValueRef range[2];
  range[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  range[1] = LLVMConstInt(LLVMInt32Type(), 0, 0);

  LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 15), "simple_value");
  LLVMValueRef simple_value_pointer = LLVMBuildInBoundsGEP(builder, GlobalVar, range, 2, "simple_value");
  LLVMSetInitializer(GlobalVar, LLVMConstString("\nHello World!", 15, 1));
  LLVMBuildCall(builder, puts_fn, &simple_value_pointer, 1, "");

  LLVMBasicBlockRef entry_block = LLVMGetInsertBlock(builder);
  LLVMBasicBlockRef left_block = LLVMAppendBasicBlockInContext(context, main, "left");
  LLVMBasicBlockRef right_block = LLVMAppendBasicBlockInContext(context, main, "right");


  LLVMValueRef val = LLVMBuildAlloca(builder, LLVMInt32Type(), "ret_val");
  LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 11, 0), val);


  LLVMBuildCondBr(builder, LLVMConstInt(LLVMInt1TypeInContext(context), 1, 0), left_block, right_block);

  LLVMPositionBuilderAtEnd(builder, left_block);

  LLVMBuildStore(builder, LLVMConstInt(LLVMInt32Type(), 22, 0), val);
  LLVMBuildBr(builder, right_block);
  LLVMPositionBuilderAtEnd(builder, right_block);

  LLVMBuildRet(builder, LLVMBuildLoad(builder, val, "ret_val"));

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

  emit_file(mod, "abc.ll");
  ho();

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

LLVMValueRef codegen_string(LLVMModuleRef module, LLVMContextRef context, const char* str, size_t len)
{
  LLVMValueRef str_val = LLVMConstStringInContext(context, str, (int)len, 0);
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
