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

#define DESC_VTABLE 16

int main(int argc, char const *argv[]) {
  LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

  LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
  LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
  LLVMValueRef sum = LLVMAddFunction(mod, "main", ret_type);

  // what is an address space?
  LLVMTypeRef nyans[] = { LLVMPointerType(LLVMInt8Type(), 0) };
  LLVMTypeRef rettype = LLVMFunctionType(LLVMInt32Type(), nyans, 1, 0);
  LLVMValueRef puts_fn = LLVMAddFunction(mod, "puts", rettype);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

  LLVMContextRef context = LLVMGetGlobalContext();
  LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
  LLVMPositionBuilderAtEnd(builder, entry);

  LLVMTypeRef te = LLVMInt32Type();
  LLVMValueRef val = LLVMBuildAlloca(builder, te, "uei");
  LLVMBuildStore(builder, LLVMConstInt(te, 31, 0), val);
  LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMBuildLoad(builder, val, "uei"), "tmp");

  LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 6), "simple_value");
  LLVMValueRef TempStr = LLVMConstString("nyan", 6, 1);
  LLVMSetInitializer(GlobalVar, TempStr);

  LLVMValueRef gep[2];
  gep[0] = LLVMConstInt(LLVMInt32Type(), 0, 0);
  gep[1] = LLVMConstInt(LLVMInt32Type(), 0, 0);

  LLVMValueRef aaa = LLVMBuildInBoundsGEP(builder, GlobalVar, gep, 2, "simple_value");
  LLVMBuildCall(builder, puts_fn, &aaa, 1, "puts");

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

  LLVMDumpModule(mod);

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
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
