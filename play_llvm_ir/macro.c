#include <stdio.h>


int main(int argc, char **argv) {
  #define POP(a)        (printf("%d", a))

  POP(5);
}