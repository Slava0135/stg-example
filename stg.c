#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>

#define PRINT_FUNCTION_NAME() printf("[%s]\n", __func__)

#define JUMP(code_label) return (CodeLabel)code_label
#define ENTER(node) JUMP(**((CodeLabel **)node))

#define TRUE 1
#define FALSE 0

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[10000];

StgWord *Node;
StgWord *RetVecReg;

int int_reg;
int bool_reg;

void push_b(StgWord value) {
  SpB[1] = value;
  SpB = SpB + 1;
}

StgWord pop_b() {
  SpB = SpB - 1;
  return SpB[1];
}

void push_a(StgWord *value) {
  SpA[-1] = value;
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
  JUMP(RetVecReg[0]);
}
CodeLabel one_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct);
}
StgWord one_info[] = {one_entry};
StgWord one_closure[] = {&one_info};

// two = {} \n {} -> Int# {2#}
CodeLabel two_direct() {
  PRINT_FUNCTION_NAME();
  int_reg = 2;
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
CodeLabel two_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(two_direct);
}
StgWord two_info[] = {two_entry};
StgWord two_closure[] = {&two_info};

// one_hundred = {} \n {} -> Int# {100#}
CodeLabel one_hundred_direct() {
  PRINT_FUNCTION_NAME();
  int_reg = 100;
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
CodeLabel one_hundred_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(one_hundred_direct);
}
StgWord one_hundred_info[] = {one_hundred_entry};
StgWord one_hundred_closure[] = {&one_hundred_info};

///// plus /////

CodeLabel plus_return_Int2() {
  PRINT_FUNCTION_NAME();
  int l = (intptr_t)pop_b();
  int r = int_reg;
  int_reg = l + r;
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
StgWord plus_return_vec2[] = {plus_return_Int2};

CodeLabel plus_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_Int2);
  push_b((StgWord)(intptr_t)int_reg); // push l
  Node = SpA[1];
  ENTER(Node); // enter r
}
StgWord plus_return_vec1[] = {plus_return_Int1};

// plus = {} \n {l,r} -> ...
CodeLabel plus_direct() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_vec1);
  Node = SpA[0];
  ENTER(Node); // enter l
}
CodeLabel plus_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(plus_direct);
}
StgWord plus_info[] = {plus_entry};
StgWord plus_closure[] = {&plus_info};

///// mult /////

CodeLabel mult_return_Int2() {
  PRINT_FUNCTION_NAME();
  int l = (intptr_t)pop_b();
  int r = int_reg;
  int_reg = l * r;
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
StgWord mult_return_vec2[] = {mult_return_Int2};

CodeLabel mult_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_Int2);
  push_b((StgWord)(intptr_t)int_reg); // push l
  Node = SpA[1];
  ENTER(Node); // enter r
}
StgWord mult_return_vec1[] = {mult_return_Int1};

// mult = {} \n {l,r} -> ...
CodeLabel mult_direct() {
  PRINT_FUNCTION_NAME();
  push_b(mult_return_vec1);
  Node = SpA[0];
  ENTER(Node); // enter l
}
CodeLabel mult_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(mult_direct);
}
StgWord mult_info[] = {mult_entry};
StgWord mult_closure[] = {&mult_info};

///// sub /////

CodeLabel sub_return_Int2() {
  PRINT_FUNCTION_NAME();
  int l = (intptr_t)pop_b();
  int r = int_reg;
  int_reg = l - r;
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
StgWord sub_return_vec2[] = {sub_return_Int2};

CodeLabel sub_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_Int2);
  push_b((StgWord)(intptr_t)int_reg); // push l
  Node = SpA[1];
  ENTER(Node); // enter r
}
StgWord sub_return_vec1[] = {sub_return_Int1};

// sub = {} \n {l,r} -> ...
CodeLabel sub_direct() {
  PRINT_FUNCTION_NAME();
  push_b(sub_return_vec1);
  Node = SpA[0];
  ENTER(Node); // enter l
}
CodeLabel sub_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(sub_direct);
}
StgWord sub_info[] = {sub_entry};
StgWord sub_closure[] = {&sub_info};

///// eq /////

CodeLabel eq_return_Int2() {
  PRINT_FUNCTION_NAME();
  int l = (intptr_t)pop_b();
  int r = int_reg;
  if (l == r) {
    bool_reg = TRUE;
  } else {
    bool_reg = FALSE;
  }
  RetVecReg = pop_b();
  JUMP(RetVecReg[0]);
}
StgWord eq_return_vec2[] = {eq_return_Int2};

CodeLabel eq_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_Int2);
  push_b((StgWord)(intptr_t)int_reg); // push l
  Node = SpA[1];
  ENTER(Node); // enter r
}
StgWord eq_return_vec1[] = {eq_return_Int1};

// eq = {} \n {l,r} -> ...
CodeLabel eq_direct() {
  PRINT_FUNCTION_NAME();
  push_b(eq_return_vec1);
  Node = SpA[0];
  ENTER(Node); // enter l
}
CodeLabel eq_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(eq_direct);
}
StgWord eq_info[] = {eq_entry};
StgWord eq_closure[] = {&eq_info};

///// pow /////

CodeLabel pow_return_Int1() {
  if (int_reg == 0) {

  } else {
  }
}
StgWord pow_return_vec1[] = {pow_return_Int1};

// pow = {} \n {e,n} -> ...
CodeLabel pow_direct() {
  PRINT_FUNCTION_NAME();
  push_b(pow_return_vec1);
  Node = SpA[1];
  ENTER(Node);
}
CodeLabel pow_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(pow_direct);
}
StgWord pow_info[] = {pow_entry};
StgWord pow_closure[] = {&pow_info};

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
