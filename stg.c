#include "stdio.h"
#include "stdlib.h"

#define PRINT_FUNCTION_NAME() printf("[%s]\n", __func__)

#define JUMP(code_label) return (CodeLabel) & code_label
#define ENTER(node) JUMP(***((CodeLabel ***)node))

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[10000];

StgWord *Node;
StgWord **RetVecReg;

int int_reg;

void push_b(void *value) {
  SpB[1] = (StgWord)value;
  SpB = SpB + 1;
}

StgWord pop_b() {
  SpB = SpB - 1;
  return SpB[1];
}

void push_a(void *value) {
  SpA[-1] = (StgWord *)value;
  SpA = SpA - 1;
}

StgWord *pop_a() {
  SpA = SpA + 1;
  return SpB[-1];
}

///// constants /////

// one = {} \n {} -> Int# {1#}
CodeLabel one_direct() {
  PRINT_FUNCTION_NAME();
  int_reg = 1;
  RetVecReg = pop_b();
  JUMP(*RetVecReg[0]);
}
CodeLabel one_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct);
}
StgWord one_info[] = {one_entry};
StgWord one_closure[] = {&one_info};

///// plus /////

CodeLabel plus_return_Int2() {
  PRINT_FUNCTION_NAME();
  ENTER(Node);
}
StgWord plus_return_vec2[] = {plus_return_Int2};

CodeLabel plus_return_Int1() {
  PRINT_FUNCTION_NAME();
  ENTER(Node);
}
StgWord plus_return_vec1[] = {plus_return_Int1};

/* plus = {} \n {l,r} -> ... */
CodeLabel plus_direct() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_vec1);
  Node = SpA[0];
  ENTER(Node);
}
CodeLabel plus_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(plus_direct);
}
StgWord plus_info[] = {plus_entry};
StgWord plus_closure[] = {&plus_info};

///// main /////

CodeLabel return_int() {
  PRINT_FUNCTION_NAME();
  printf("Result = %d\n", int_reg);
  exit(0);
}
StgWord return_int_return_vec[] = {return_int};

CodeLabel main_direct() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct);
}
CodeLabel main_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(main_direct);
}
StgWord main_info[] = {main_entry};
StgWord *main_closure[] = {(StgWord *)&main_info};

int main() {
  push_b(return_int_return_vec);
  CodeLabel cont = (CodeLabel)&main_entry;
  while (1) {
    cont = (*cont)();
  }
  return 0;
}
