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

int IntReg;
StgWord VarIdReg;
StgWord ExprReg1;
StgWord ExprReg2;
StgWord ExprReg3;

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
  IntReg = 1;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
CodeLabel one_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct);
}
StgWord one_info[] = {one_entry};

// two = {} \n {} -> Int# {2#}
CodeLabel two_direct() {
  PRINT_FUNCTION_NAME();
  IntReg = 2;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
CodeLabel two_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(two_direct);
}
StgWord two_info[] = {two_entry};

// one_hundred = {} \n {} -> Int# {100#}
CodeLabel one_hundred_direct() {
  PRINT_FUNCTION_NAME();
  IntReg = 100;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
CodeLabel one_hundred_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(one_hundred_direct);
}
StgWord one_hundred_info[] = {one_hundred_entry};

///// plus /////

CodeLabel plus_return_Int2() {
  PRINT_FUNCTION_NAME();
  int lI = (intptr_t)pop_b();
  int rI = IntReg;
  IntReg = lI + rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
StgWord plus_return_vec2[] = {plus_return_Int2};

CodeLabel plus_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_vec2);
  push_b((StgWord)(intptr_t)IntReg); // push l
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

///// mult /////

CodeLabel mult_return_Int2() {
  PRINT_FUNCTION_NAME();
  int lI = (intptr_t)pop_b();
  int rI = IntReg;
  IntReg = lI * rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
StgWord mult_return_vec2[] = {mult_return_Int2};

CodeLabel mult_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(mult_return_vec2);
  push_b((StgWord)(intptr_t)IntReg); // push l
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

///// sub /////

CodeLabel sub_return_Int2() {
  PRINT_FUNCTION_NAME();
  int lI = (intptr_t)pop_b();
  int rI = IntReg;
  IntReg = lI - rI;
  RetVecReg = pop_b();
  pop_a();                  // pop l
  pop_a();                  // pop r
  JUMP(RetVecReg[RET_INT]); // continue with Int#
}
StgWord sub_return_vec2[] = {sub_return_Int2};

CodeLabel sub_return_Int1() {
  PRINT_FUNCTION_NAME();
  push_b(sub_return_vec2);
  push_b((StgWord)(intptr_t)IntReg); // push l
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

///// eq /////

CodeLabel eq_return_Int2() {
  PRINT_FUNCTION_NAME();
  int lI = (intptr_t)pop_b();
  int rI = IntReg;
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
  push_b(eq_return_vec2);
  push_b((StgWord)(intptr_t)IntReg); // push l
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

///// id_eq /////

CodeLabel id_eq_return_VarId2() {
  PRINT_FUNCTION_NAME();
  StgWord l = pop_b();
  StgWord r = VarIdReg;
  pop_a(); // pop l_id
  pop_a(); // pop r_id
  push_a(r);
  push_a(l);
  JUMP(eq_direct); // static
}
StgWord id_eq_return_vec2[] = {id_eq_return_VarId2};

CodeLabel id_eq_return_VarId1() {
  PRINT_FUNCTION_NAME();
  push_b(id_eq_return_vec2);
  push_b(VarIdReg); // push l
  Node = SpA[1];
  ENTER(Node); // enter r_id
}
StgWord id_eq_return_vec1[] = {id_eq_return_VarId1};

// id_eq = {} \n {l_id,r_id} -> ...
CodeLabel id_eq_direct() {
  PRINT_FUNCTION_NAME();
  push_b(id_eq_return_vec1);
  Node = SpA[0];
  ENTER(Node); // enter l_id
}
CodeLabel id_eq_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(id_eq_direct);
}
StgWord id_eq_info[] = {id_eq_entry};

///// eval /////

// Lit {n} -> n {}
CodeLabel eval_go_return_lit() {
  PRINT_FUNCTION_NAME();
  pop_a(); // pop expr
  StgWord n = ExprReg1;
  Node = n;
  ENTER(n);
}
// Var {x} -> valueOf {x}
CodeLabel eval_go_return_var() {
  PRINT_FUNCTION_NAME();
  StgWord *valueOf = Node[1];
  pop_a(); // pop expr
  StgWord x = ExprReg1;
  push_a(x);
  Node = valueOf;
  ENTER(Node);
}

// go_l = {go,l} \u {} -> go {l}
// go_r = {go,r} \u {} -> go {r}
CodeLabel eval_go_go_lr_entry() {
  PRINT_FUNCTION_NAME();
  StgWord go = Node[1];
  StgWord lr = Node[2];
  push_a(lr);
  Node = (StgWord *)go;
  ENTER(Node);
}
StgWord eval_go_go_lr_info[] = {eval_go_go_lr_entry};

void eval_go_return_add_mul_common() {
  StgWord *go = Node[2];
  StgWord l = ExprReg1;
  StgWord r = ExprReg2;
  // fill closure go_l
  allocate(eval_go_go_lr_info);
  StgWord *go_l = Hp;
  allocate(go);
  allocate(l);
  // fill closure go_r
  allocate(eval_go_go_lr_info);
  StgWord *go_r = Hp;
  allocate(go);
  allocate(r);
  // call plus/mult
  pop_a(); // pop expr
  push_a(go_r);
  push_a(go_l);
}

// Add {l,r} -> ...
CodeLabel eval_go_return_add() {
  PRINT_FUNCTION_NAME();
  eval_go_return_add_mul_common();
  JUMP(plus_direct); // static
}
// Mul {l,r} -> ...
CodeLabel eval_go_return_mul() {
  PRINT_FUNCTION_NAME();
  eval_go_return_add_mul_common();
  JUMP(mult_direct); // static
}

// Let {x,e,body} -> ...
CodeLabel eval_go_return_let() { PRINT_FUNCTION_NAME(); }

StgWord eval_go_return_vec[] = {eval_go_return_lit, eval_go_return_var,
                                eval_go_return_add, eval_go_return_mul,
                                eval_go_return_let};

// go = {valueOf,go} \n {expr} ->
CodeLabel eval_go_entry() {
  PRINT_FUNCTION_NAME();
  Node = SpA[0];
  push_b(eval_go_return_vec);
  ENTER(Node);
}
StgWord eval_go_info[] = {eval_go_entry};

// eval = {} \n {valueOf,expr} -> ...
CodeLabel eval_direct() {
  PRINT_FUNCTION_NAME();
  StgWord *valueOf = SpA[0];
  StgWord *expr = SpA[1];
  // fill closure go
  allocate(eval_go_info);
  StgWord *go = Hp;
  allocate((StgWord)valueOf);
  allocate((StgWord)go);
  // call go
  Node = go;
  push_a(expr);
  ENTER(Node);
}
CodeLabel eval_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(eval_direct);
}
StgWord eval_info[] = {eval_entry};

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
  push_a(one_info);
  push_a(n);
  JUMP(sub_direct); // static
}
StgWord pow_ns_info[] = {pow_ns_entry};

CodeLabel pow_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (IntReg == 0) {
    ExprReg1 = (StgWord)1;
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
    ExprReg1 = (StgWord)ns;
    ExprReg2 = (StgWord)pows;
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
  ExprReg1 = var_z;
  ExprReg2 = sops;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_ADD]);
}
StgWord sop_add_info[] = {sops_add_entry};

// var_z = {z} \n {} -> Var {z}
CodeLabel sops_var_z_entry() {
  PRINT_FUNCTION_NAME();
  StgWord z = Node[1];
  ExprReg1 = z;
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
  VarIdReg = n;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR_ID]);
}
StgWord sop_z_info[] = {sops_z_entry};

// n' = {n} \u {} -> sub {n,one}
CodeLabel sops_ns_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  push_a(one_info);
  push_a(n);
  JUMP(sub_direct); // static
}
StgWord sop_ns_info[] = {sops_ns_entry};

CodeLabel sop_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (IntReg == 0) {
    ExprReg1 = (StgWord)1;
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
    ExprReg1 = (StgWord)z;
    ExprReg2 = (StgWord)pows;
    ExprReg3 = (StgWord)add;
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
  printf("Result = %d\n", IntReg);
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
