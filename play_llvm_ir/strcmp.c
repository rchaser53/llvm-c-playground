#include <stdio.h>
#include <string.h>

int for_llvm_strcmp(char *a, char *b)
{
    return strcmp(a, b);
}

int main(void)
{
  char *a1 = "abc";
  char *a2 = "abc";
  int result = for_llvm_strcmp(a1, a2);

  printf("%d", result);
  return 0;
}