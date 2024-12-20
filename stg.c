#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>

#define PRINT_FUNCTION_NAME() printf("[%s]\n", __func__)

#define JUMP(code_label) return (CodeLabel)code_label
#define ENTER(node) JUMP(**((CodeLabel **)node))

#define RET_INT 0

#define RET_VAR_ID 0

#define RET_TRUE 0
#define RET_FALSE 1

#define RET_LIT 0
#define RET_VAR 1
#define RET_ADD 2
#define RET_MUL 3
#define RET_LET 4

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[10000];

StgWord heap[10000];
StgWord *Hp = heap;

StgWord *Node;
StgWord *RetVecReg;

int int_reg;
StgWord var_id_reg;
StgWord expr_reg1;
StgWord expr_reg2;
StgWord expr_reg3;

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

void allocate(StgWord value) {
  Hp[1] = value;
  Hp = Hp + 1;
}

///// constants /////

// one = {} \n {} -> Int# {1#}
CodeLabel one_direct() {
  PRINT_FUNCTION_NAME();
  int_reg = 1;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  int lI = (intptr_t)pop_b();
  int rI = int_reg;
  int_reg = lI + rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  int lI = (intptr_t)pop_b();
  int rI = int_reg;
  int_reg = lI * rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  int lI = (intptr_t)pop_b();
  int rI = int_reg;
  int_reg = lI - rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
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
  int lI = (intptr_t)pop_b();
  int rI = int_reg;
  RetVecReg = pop_b();
  pop_a(); // pop l
  pop_a(); // pop r
  if (lI == rI) {
    JUMP(RetVecReg[RET_TRUE]); // continue with TRUE
  } else {
    JUMP(RetVecReg[RET_FALSE]); // continue with FALSE
  }
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

CodeLabel pow_direct();

// pow' = {e,n'} \u {} -> pow {e,n'}
CodeLabel pow_pows_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord ns = Node[2];
  push_a(ns);
  push_a(e);
  JUMP(pow_direct); // static
}
StgWord pow_pows_info[] = {pow_pows_entry};

// n' = {n} \u {} -> sub {n,one}
CodeLabel pow_ns_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  push_a(one_closure);
  push_a(n);
  JUMP(sub_direct); // static
}
StgWord pow_ns_info[] = {pow_ns_entry};

CodeLabel pow_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (int_reg == 0) {
    expr_reg1 = (StgWord)1;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_LIT]); // continue with Lit
  } else {
    StgWord e = Node[0];
    StgWord n = Node[1];
    // fill closure n'
    allocate(pow_ns_info);
    StgWord *ns = Hp;
    allocate(n);
    // fill closure pow'
    allocate(pow_pows_info);
    StgWord *pows = Hp;
    allocate(e);
    allocate((StgWord)ns);
    // return
    expr_reg1 = (StgWord)ns;
    expr_reg2 = (StgWord)pows;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_MUL]); // continue with Mul
  }
}
StgWord pow_return_vec1[] = {pow_return_Int1};

// pow = {} \n {e,n} -> ...
CodeLabel pow_direct() {
  PRINT_FUNCTION_NAME();
  push_b(pow_return_vec1);
  Node = SpA[1];
  ENTER(Node); // enter n
}
CodeLabel pow_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(pow_direct);
}
StgWord pow_info[] = {pow_entry};

///// sop /////

CodeLabel sop_direct();

// pow' = {e,n} \u {} -> pow' {e,n}
CodeLabel sops_pows_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord ns = Node[2];
  push_a(ns);
  push_a(e);
  JUMP(pow_direct); // static
}
StgWord sop_pows_info[] = {sops_pows_entry};

// add = {var_z,sop'} \n {} Add {var_z,sop'}
CodeLabel sops_add_entry() {
  PRINT_FUNCTION_NAME();
  StgWord var_z = Node[1];
  StgWord sops = Node[2];
  expr_reg1 = var_z;
  expr_reg2 = sops;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_ADD]);
}
StgWord sop_add_info[] = {sops_add_entry};

// var_z = {z} \n {} -> Var {z}
CodeLabel sops_var_z_entry() {
  PRINT_FUNCTION_NAME();
  StgWord z = Node[1];
  expr_reg1 = z;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR]);
}
StgWord sop_var_z_info[] = {sops_var_z_entry};

// sop' = {e,n'} \u {} -> sop {e,n'}
CodeLabel sops_sops_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord ns = Node[2];
  push_a(ns);
  push_a(e);
  JUMP(sop_direct); // static
}
StgWord sop_sops_info[] = {sops_sops_entry};

// z = {n} \n {} -> VarId {n}
CodeLabel sops_z_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  var_id_reg = n;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR_ID]);
}
StgWord sop_z_info[] = {sops_z_entry};

// n' = {n} \u {} -> sub {n,one}
CodeLabel sops_ns_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  push_a(one_closure);
  push_a(n);
  JUMP(sub_direct); // static
}
StgWord sop_ns_info[] = {sops_ns_entry};

CodeLabel sop_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (int_reg == 0) {
    expr_reg1 = (StgWord)1;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_LIT]); // continue with Lit
  } else {
    StgWord e = Node[0];
    StgWord n = Node[1];
    // fill closure z
    allocate(sop_z_info);
    StgWord *z = Hp;
    allocate(n);
    // fill closure n'
    allocate(sop_ns_info);
    StgWord *ns = Hp;
    allocate(n);
    // fill closure sop'
    allocate(sop_sops_info);
    StgWord *sops = Hp;
    allocate(e);
    allocate((StgWord)ns);
    // fill closure var_z
    allocate(sop_var_z_info);
    StgWord *var_z = Hp;
    allocate(z);
    // fill closure add
    allocate(sop_add_info);
    StgWord *add = Hp;
    allocate(var_z);
    allocate(sops);
    // fill closure pow'
    allocate(sop_pows_info);
    StgWord *pows = Hp;
    allocate(e);
    allocate((StgWord)n);
    // return
    expr_reg1 = (StgWord)z;
    expr_reg2 = (StgWord)pows;
    expr_reg3 = (StgWord)add;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_LET]); // continue with Let
  }
}
StgWord sop_return_vec1[] = {sop_return_Int1};

// sop = {} \n {e,n} -> ...
CodeLabel sop_direct() {
  PRINT_FUNCTION_NAME();
  push_b(sop_return_vec1);
  Node = SpA[1];
  ENTER(Node); // enter n
}
CodeLabel sop_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(sop_direct);
}
StgWord sop_info[] = {sop_entry};

///// main /////

CodeLabel return_int() {
  PRINT_FUNCTION_NAME();
  printf("Result = %d\n", int_reg);
  exit(0);
}
StgWord return_int_return_vec[] = {return_int};

CodeLabel main_direct() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct); // static
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
