
@0 = private unnamed_addr constant [8 x i8] c"%d\0A\00fai\00"
@hello = constant [6 x i8] c"hello\00"
@hell = constant [5 x i8] c"hell\00"
@hello_cpy = constant [6 x i8] c"hello\00"

declare i32 @strcmp(i8*, i8*)
declare i32 @printf(...)

define i32 @main() #0 {
  %str1 = getelementptr inbounds [5 x i8], [5 x i8]* @hell, i32 0, i32 0
  %str2 = getelementptr inbounds [6 x i8], [6 x i8]* @hello, i32 0, i32 0

  %str3 = getelementptr inbounds [6 x i8], [6 x i8]* @hello_cpy, i32 0, i32 0
  ; %ret_strcmp = call i32 @strcmp(i8* %str1, i8* %str2)
  ; %ret_strcmp = call i32 @strcmp(i8* %str2, i8* %str3)

  %1 = call i32 (...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @0, i32 0, i32 0), i32 %ret_strcmp)

  ret i32 0
}