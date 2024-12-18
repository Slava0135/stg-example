#include "stdio.h"
#include "stdlib.h"

#define PRINT_NAME() printf("[%s]\n", __func__)
#define JUMP(code_label) return (CodeLabel) & code_label
#define ENTER(node) JUMP((*((CodeLabel **)node[0]))[0])

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[10000];

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

CodeLabel main_direct() {
  PRINT_NAME();
  exit(0);
}
CodeLabel main_entry() {
  PRINT_NAME();
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
