#include "stdio.h"
#include "stdlib.h"

#define PRINT_FUNCTION_NAME() printf("[%s]\n", __func__)
#define JUMP(code_label) return (CodeLabel) & code_label
#define ENTER(node) JUMP((*((CodeLabel **)node[0]))[0])

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[10000];

StgWord *Node;

void push_b(void *value) {
  SpB[0] = (StgWord)value;
  SpB = SpB + 1;
}

StgWord pop_b() {
  SpB = SpB - 1;
  return SpB[0];
}

void push_a(void *value) {
  SpA[-1] = (StgWord *)value;
  SpA = SpA - 1;
}

StgWord *pop_a() {
  SpA = SpA + 1;
  return SpB[-1];
}

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
  push_b(plus_return_vec1); // push case alternatives on return stack
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

CodeLabel main_direct() {
  PRINT_FUNCTION_NAME();
  exit(0);
}
CodeLabel main_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(main_direct);
}
StgWord main_info[] = {main_entry};
StgWord *main_closure[] = {(StgWord *)&main_info};

int main() {
  CodeLabel cont = (CodeLabel)&main_entry;
  while (1) {
    cont = (*cont)();
  }
  return 0;
}
