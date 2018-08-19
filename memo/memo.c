#include <stdio.h>

struct RT {
  char A;
  // int B[10][20];
  // int B[10][20];
  int B[5];
  char C;
};
struct ST {
  int X;
  double Y;
  struct RT Z;
};

int * foo(struct ST *s) {
  // return s[1].Z.B[5][13];
  // return &s[1].Z.B[4];
  return &s[0].Z.B[4];
}

int main(int argc, char const *argv[]) {

  struct RT rt = {
    // 'a', {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}}, 'c'
    'a', {0, 1, 2, 3, 4}, 'c'
  };
  
  // rt.B[5][13] = 234;
  // rt.A = 'a';
  // rt.B = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}};

  // struct RT * rt_pointer = &rt;
  // int * b_item_pointer = &rt_pointer[0].B[2];
  // printf("%d\n", *b_item_pointer);
  
  struct ST st = { 123, 12, rt };
  int * st_pointer_int = foo(&st);

  printf("%d\n", *st_pointer_int);



  // int b_int = rt_pointer->B[3];
  // printf("%d\n", b_int);

  // rt.C = 'c';
  // struct ST st = { 123, 12, rt };

  // int * ho = foo(&st);

  // printf("%d\n", *ho);


  // auto int *heap;
  // heap = (int *)malloc(sizeof(int) * 10);
  // heap = (int *)realloc(heap,sizeof(int) * 100);
  // heap[11] = 10;
  // printf("%d\n", heap[11]);
  // free(heap);
}