  // LLVMTypeRef te = LLVMInt32Type();
  // LLVMValueRef val = LLVMBuildAlloca(builder, te, "uei");
  // LLVMBuildStore(builder, LLVMConstInt(te, 31, 0), val);
  // LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(main, 0), LLVMBuildLoad(builder, val, "uei"), "tmp");

  // LLVMValueRef GlobalVar = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), 6), "simple_value");
  // LLVMValueRef TempStr = LLVMConstString("nyan", 6, 1);
  // LLVMSetInitializer(GlobalVar, TempStr);




  // LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMPointerType(LLVMPointerType(LLVMInt8Type(), 0), 0) };
  // LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 2, 0);
  // LLVMTypeRef param_types[] = { LLVMInt32Type() };
  // LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32Type(), param_types, 1, 0);
  // LLVMValueRef main = LLVMAddFunction(mod, "main", ret_type);


// LLVMValueRef llvmGenLocalStringVar(LLVMModuleRef mod, const char* data, int len) {
//   LLVMValueRef glob = LLVMAddGlobal(mod, LLVMArrayType(LLVMInt8Type(), len), "string");

//   LLVMSetLinkage(glob, LLVMInternalLinkage);
//   LLVMSetGlobalConstant(glob, 1);
//   LLVMSetInitializer(glob, LLVMConstString(data, len, 1));

//   return glob;
// }
