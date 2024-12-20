#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PRINT_FUNCTION_NAME()                                                  \
  printf("%s:%d:[%s]\n", __FILE__, __LINE__, __func__)
#define NAME_OF(var) #var

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

#define STACK_SIZE 10000

typedef void *(*CodeLabel)();
typedef void *StgWord;

StgWord Sp[STACK_SIZE];
StgWord *SpB = Sp;
StgWord **SpA = (StgWord **)&Sp[STACK_SIZE];

StgWord heap[STACK_SIZE];
StgWord *Hp = heap;

StgWord *Node;
StgWord *RetVecReg;

int IntReg;
StgWord VarIdReg;
StgWord ExprReg1;
StgWord ExprReg2;
StgWord ExprReg3;

const char *DebugSp[STACK_SIZE];
const char **DebugSpB = DebugSp;
const char **DebugSpA = &DebugSp[STACK_SIZE];

void push_b(StgWord value, const char *debug_info) {
  SpB[1] = value;
  SpB = SpB + 1;
  DebugSpB[1] = debug_info;
  DebugSpB = DebugSpB + 1;
  printf("\tpushB: %x '%s'\n", value, debug_info);
}

StgWord pop_b() {
  SpB = SpB - 1;
  DebugSpB = DebugSpB - 1;
  StgWord value = SpB[1];
  const char *debug_info = DebugSpB[1];
  printf("\tpopB: %x '%s'\n", value, debug_info);
  return value;
}

void push_a(StgWord *value, const char *debug_info) {
  SpA[-1] = value;
  SpA = SpA - 1;
  DebugSpA[-1] = debug_info;
  DebugSpA = DebugSpA - 1;
  printf("\tpushA: %x '%s'\n", value, debug_info);
}

StgWord *pop_a() {
  SpA = SpA + 1;
  DebugSpA = DebugSpA + 1;
  StgWord value = SpA[-1];
  const char *debug_info = DebugSpA[-1];
  printf("\tpopA: %x '%s'\n", value, debug_info);
  return value;
}

StgWord *arg(int offset) {
  StgWord value = SpA[offset];
  const char *debug_info = DebugSpA[offset];
  printf("\targument(%d): %x '%s'\n", offset, value, debug_info);
  return value;
}

int nodes_saved = 0;
void save_node() {
  nodes_saved++;
  push_b(Node, "save node");
}
void load_node() {
  assert(nodes_saved > 0);
  nodes_saved--;
  Node = pop_b();
}

StgWord *allocate(int size) {
  StgWord ptr = Hp;
  Hp = Hp + size;
  return ptr;
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
StgWord one_closure[] = {one_info};

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
StgWord two_closure[] = {two_info};

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
StgWord one_hundred_closure[] = {one_hundred_info};

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
  push_b((StgWord)(intptr_t)IntReg, "l");
  push_b(plus_return_vec2, NAME_OF(plus_return_vec2));
  Node = arg(1);
  ENTER(Node); // enter r
}
StgWord plus_return_vec1[] = {plus_return_Int1};

// plus = {} \n {l,r} -> ...
CodeLabel plus_direct() {
  PRINT_FUNCTION_NAME();
  push_b(plus_return_vec1, NAME_OF(plus_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter l
}
CodeLabel plus_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(plus_direct);
}
StgWord plus_info[] = {plus_entry};
StgWord plus_closure[] = {plus_info};

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
  push_b((StgWord)(intptr_t)IntReg, "l");
  push_b(mult_return_vec2, NAME_OF(mult_return_vec2));
  Node = arg(1);
  ENTER(Node); // enter r
}
StgWord mult_return_vec1[] = {mult_return_Int1};

// mult = {} \n {l,r} -> ...
CodeLabel mult_direct() {
  PRINT_FUNCTION_NAME();
  push_b(mult_return_vec1, NAME_OF(mult_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter l
}
CodeLabel mult_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(mult_direct);
}
StgWord mult_info[] = {mult_entry};
StgWord mult_closure[] = {mult_info};

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
  push_b((StgWord)(intptr_t)IntReg, "l");
  push_b(sub_return_vec2, NAME_OF(sub_return_vec2));
  Node = arg(1);
  ENTER(Node); // enter r
}
StgWord sub_return_vec1[] = {sub_return_Int1};

// sub = {} \n {l,r} -> ...
CodeLabel sub_direct() {
  PRINT_FUNCTION_NAME();
  push_b(sub_return_vec1, NAME_OF(sub_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter l
}
CodeLabel sub_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(sub_direct);
}
StgWord sub_info[] = {sub_entry};
StgWord sub_closure[] = {sub_info};

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
  push_b((StgWord)(intptr_t)IntReg, "l");
  push_b(eq_return_vec2, NAME_OF(eq_return_vec2));
  Node = arg(1);
  ENTER(Node); // enter r
}
StgWord eq_return_vec1[] = {eq_return_Int1};

// eq = {} \n {l,r} -> ...
CodeLabel eq_direct() {
  PRINT_FUNCTION_NAME();
  push_b(eq_return_vec1, NAME_OF(eq_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter l
}
CodeLabel eq_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(eq_direct);
}
StgWord eq_info[] = {eq_entry};
StgWord eq_closure[] = {eq_info};

///// id_eq /////

CodeLabel id_eq_return_VarId2() {
  PRINT_FUNCTION_NAME();
  StgWord l = pop_b();
  StgWord r = VarIdReg;
  pop_a(); // pop l_id
  pop_a(); // pop r_id
  push_a(r, NAME_OF(r));
  push_a(l, NAME_OF(l));
  JUMP(eq_direct); // static
}
StgWord id_eq_return_vec2[] = {id_eq_return_VarId2};

CodeLabel id_eq_return_VarId1() {
  PRINT_FUNCTION_NAME();
  push_b(VarIdReg, "l");
  push_b(id_eq_return_vec2, NAME_OF(id_eq_return_vec2));
  Node = arg(1);
  ENTER(Node); // enter r_id
}
StgWord id_eq_return_vec1[] = {id_eq_return_VarId1};

// id_eq = {} \n {l_id,r_id} -> ...
CodeLabel id_eq_direct() {
  PRINT_FUNCTION_NAME();
  push_b(id_eq_return_vec1, NAME_OF(id_eq_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter l_id
}
CodeLabel id_eq_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(id_eq_direct);
}
StgWord id_eq_info[] = {id_eq_entry};
StgWord id_eq_closure[] = {id_eq_info};

///// eval /////

CodeLabel eval_direct();

// Lit {n} -> n {}
CodeLabel eval_go_return_lit() {
  PRINT_FUNCTION_NAME();
  load_node();
  pop_a(); // pop expr
  StgWord n = ExprReg1;
  Node = n;
  ENTER(n);
}
// Var {x} -> valueOf {x}
CodeLabel eval_go_return_var() {
  PRINT_FUNCTION_NAME();
  load_node();
  StgWord *valueOf = Node[1];
  pop_a(); // pop expr
  StgWord x = ExprReg1;
  push_a(x, NAME_OF(x));
  Node = valueOf;
  ENTER(Node);
}

// go_l = {go,l} \u {} -> go {l}
// go_r = {go,r} \u {} -> go {r}
// n = {go,e} \u {} -> go {e}
CodeLabel eval_go_go_lrn_entry() {
  PRINT_FUNCTION_NAME();
  StgWord *go = Node[1];
  StgWord lrn = Node[2];
  push_a(lrn, NAME_OF(lrn));
  Node = go;
  ENTER(Node);
}
StgWord eval_go_go_lrn_info[] = {eval_go_go_lrn_entry};

void eval_go_return_add_mul_common() {
  load_node();
  StgWord *go = Node[2];
  StgWord l = ExprReg1;
  StgWord r = ExprReg2;
  // fill closure go_l = {go,l} \u {} -> go {l}
  StgWord *go_l_closure = allocate(3);
  go_l_closure[0] = eval_go_go_lrn_info;
  go_l_closure[1] = go;
  go_l_closure[2] = l;
  // fill closure go_r = {go,r} \u {} -> go {r}
  StgWord *go_r_closure = allocate(3);
  go_r_closure[0] = eval_go_go_lrn_info;
  go_r_closure[1] = go;
  go_r_closure[2] = r;
  // call plus/mult {go_l,go_r}
  pop_a(); // pop expr
  push_a(go_r_closure, NAME_OF(go_r_closure));
  push_a(go_l_closure, NAME_OF(go_l_closure));
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

// True {} -> n {}
CodeLabel eval_go_let_valueOfs_return_True() {
  PRINT_FUNCTION_NAME();
  load_node();
  pop_a(); // pop y
  StgWord n = Node[2];
  Node = n;
  ENTER(n);
}
// _ -> valueOf {y}
CodeLabel eval_go_let_valueOfs_return_False() {
  PRINT_FUNCTION_NAME();
  load_node();
  StgWord *valueOf = Node[3];
  StgWord *y = arg(0);
  pop_a(); // pop y
  push_a(y, NAME_OF(y));
  Node = valueOf;
  ENTER(Node);
}
StgWord eval_go_let_valueOfs_return_vec[] = {eval_go_let_valueOfs_return_True,
                                             eval_go_let_valueOfs_return_False};
// valueOf' = {x,n,valueOf} \n {y} -> case id_eq {x,y}
CodeLabel eval_go_let_valueOfs_entry() {
  PRINT_FUNCTION_NAME();
  StgWord x = Node[1];
  StgWord y = arg(0);
  push_a(y, NAME_OF(y));
  push_a(x, NAME_OF(x));
  save_node();
  push_b(eval_go_let_valueOfs_return_vec,
         NAME_OF(eval_go_let_valueOfs_return_vec));
  JUMP(id_eq_direct); // static
}
StgWord eval_go_let_valueOfs_info[] = {eval_go_let_valueOfs_entry};
// Let {x,e,body} -> ...
CodeLabel eval_go_return_let() {
  PRINT_FUNCTION_NAME();
  load_node();
  StgWord *valueOf = Node[1];
  StgWord *go = Node[2];
  StgWord x = ExprReg1;
  StgWord e = ExprReg2;
  StgWord body = ExprReg3;
  // fill closure n = {go,e} \u {} -> go {e}
  StgWord *n_closure = allocate(3);
  n_closure[0] = eval_go_go_lrn_info;
  n_closure[1] = go;
  n_closure[2] = e;
  // fill closure valueOf' = {x,n,valueOf} \n {y} -> case id_eq {x,y} of
  StgWord *valueOfs_closure = allocate(4);
  valueOfs_closure[0] = eval_go_let_valueOfs_info;
  valueOfs_closure[1] = x;
  valueOfs_closure[2] = n_closure;
  valueOfs_closure[3] = valueOf;
  // call eval {valueOf',body}
  pop_a(); // pop expr
  push_a(body, NAME_OF(body));
  push_a(valueOfs_closure, NAME_OF(valueOfs_closure));
  JUMP(eval_direct); // static
}

StgWord eval_go_return_vec[] = {eval_go_return_lit, eval_go_return_var,
                                eval_go_return_add, eval_go_return_mul,
                                eval_go_return_let};

// go = {valueOf,go} \n {expr} ->
CodeLabel eval_go_entry() {
  PRINT_FUNCTION_NAME();
  save_node();
  Node = arg(0);
  push_b(eval_go_return_vec, NAME_OF(eval_go_return_vec));
  ENTER(Node);
}
StgWord eval_go_info[] = {eval_go_entry};

// eval = {} \n {valueOf,expr} -> ...
CodeLabel eval_direct() {
  PRINT_FUNCTION_NAME();
  StgWord *valueOf = arg(0);
  StgWord *expr = arg(1);
  // fill closure go = {valueOf,go} \n {expr} -> case expr {}
  StgWord *go_closure = allocate(3);
  go_closure[0] = eval_go_info;
  go_closure[1] = valueOf;
  go_closure[2] = go_closure;
  // call go {expr}
  pop_a(); // pop valueOf
  pop_a(); // pop expr
  Node = go_closure;
  push_a(expr, NAME_OF(expr));
  ENTER(Node);
}
CodeLabel eval_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(eval_direct);
}
StgWord eval_info[] = {eval_entry};
StgWord eval_closure[] = {eval_info};

///// pow /////

CodeLabel pow_direct();

// pow' = {e,n'} \u {} -> pow {e,n'}
CodeLabel pow_pows_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord ns = Node[2];
  push_a(ns, NAME_OF(ns));
  push_a(e, NAME_OF(e));
  JUMP(pow_direct); // static
}
StgWord pow_pows_info[] = {pow_pows_entry};

// n' = {n} \u {} -> sub {n,one}
CodeLabel pow_ns_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  push_a(one_closure, NAME_OF(one_closure));
  push_a(n, NAME_OF(n));
  JUMP(sub_direct); // static
}
StgWord pow_ns_info[] = {pow_ns_entry};

CodeLabel pow_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (IntReg == 0) {
    ExprReg1 = one_closure;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_LIT]); // continue with Lit
  } else {
    StgWord e = arg(0);
    StgWord n = arg(1);
    // fill closure n' = {n} \u {} -> sub {n,one}
    StgWord *ns_closure = allocate(2);
    ns_closure[0] = pow_ns_info;
    ns_closure[1] = n;
    // fill closure pow' = {e,n'} \u {} -> pow {e,n'}
    StgWord *pows_closure = allocate(3);
    pows_closure[0] = pow_pows_info;
    pows_closure[1] = e;
    pows_closure[2] = ns_closure;
    // return
    ExprReg1 = e;
    ExprReg2 = pows_closure;
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
  push_b(pow_return_vec1, NAME_OF(pow_return_vec1));
  Node = arg(1);
  ENTER(Node); // enter n
}
CodeLabel pow_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(pow_direct);
}
StgWord pow_info[] = {pow_entry};
StgWord pow_closure[] = {pow_info};

///// sop /////

CodeLabel sop_direct();

// pow' = {e,n} \u {} -> pow {e,n}
CodeLabel sops_pows_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord n = Node[2];
  push_a(n, NAME_OF(n));
  push_a(e, NAME_OF(e));
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
  JUMP(RetVecReg[RET_ADD]); // continue with Add
}
StgWord sop_add_info[] = {sops_add_entry};

// var_z = {z} \n {} -> Var {z}
CodeLabel sops_var_z_entry() {
  PRINT_FUNCTION_NAME();
  StgWord z = Node[1];
  ExprReg1 = z;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR]); // continue with Var
}
StgWord sop_var_z_info[] = {sops_var_z_entry};

// sop' = {e,n'} \u {} -> sop {e,n'}
CodeLabel sops_sops_entry() {
  PRINT_FUNCTION_NAME();
  StgWord e = Node[1];
  StgWord ns = Node[2];
  push_a(ns, NAME_OF(ns));
  push_a(e, NAME_OF(e));
  JUMP(sop_direct); // static
}
StgWord sop_sops_info[] = {sops_sops_entry};

// z = {n} \n {} -> VarId {n}
CodeLabel sops_z_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  VarIdReg = n;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR_ID]); // continue with VarId
}
StgWord sop_z_info[] = {sops_z_entry};

// n' = {n} \u {} -> sub {n,one}
CodeLabel sops_ns_entry() {
  PRINT_FUNCTION_NAME();
  StgWord n = Node[1];
  push_a(one_closure, NAME_OF(one_closure));
  push_a(n, NAME_OF(n));
  JUMP(sub_direct); // static
}
StgWord sop_ns_info[] = {sops_ns_entry};

CodeLabel sop_return_Int1() {
  PRINT_FUNCTION_NAME();
  if (IntReg == 0) {
    ExprReg1 = one_closure;
    RetVecReg = pop_b();
    pop_a();                  // pop e
    pop_a();                  // pop n
    JUMP(RetVecReg[RET_LIT]); // continue with Lit
  } else {
    StgWord e = arg(0);
    StgWord n = arg(1);
    // fill closure z = {n} \n {} -> VarId {n}
    StgWord *z_closure = allocate(2);
    z_closure[0] = sop_z_info;
    z_closure[1] = n;
    // fill closure n' = {n} \u {} -> sub n one;
    StgWord *ns_closure = allocate(2);
    ns_closure[0] = sop_ns_info;
    ns_closure[1] = n;
    // fill closure sop' = {e,n'} \u {} -> sop {e,n'}
    StgWord *sops_closure = allocate(3);
    sops_closure[0] = sop_sops_info;
    sops_closure[1] = e;
    sops_closure[2] = ns_closure;
    // fill closure var_z = {z} \n {} -> Var {z}
    StgWord *var_z_closure = allocate(2);
    var_z_closure[0] = sop_var_z_info;
    var_z_closure[1] = z_closure;
    // fill closure add = {var_z,sop'} \n {} Add {var_z,sop'}
    StgWord *add_closure = allocate(3);
    add_closure[0] = sop_add_info;
    add_closure[1] = var_z_closure;
    add_closure[2] = sops_closure;
    // fill closure pow' = {e,n} \u {} -> pow {e,n}
    StgWord *pows_closure = allocate(3);
    pows_closure[0] = sop_pows_info;
    pows_closure[1] = e;
    pows_closure[2] = n;
    // return
    ExprReg1 = z_closure;
    ExprReg2 = pows_closure;
    ExprReg3 = add_closure;
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
  push_b(sop_return_vec1, NAME_OF(sop_return_vec1));
  Node = arg(1);
  ENTER(Node); // enter n
}
CodeLabel sop_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(sop_direct);
}
StgWord sop_info[] = {sop_entry};
StgWord sop_closure[] = {sop_info};

///// main /////

CodeLabel return_int() {
  PRINT_FUNCTION_NAME();
  printf("Result = %d\n", IntReg);
  exit(0);
}
StgWord return_int_return_vec[] = {return_int};

// x = {} \n {} -> VarId {one_hundred}
CodeLabel main_x_entry() {
  PRINT_FUNCTION_NAME();
  VarIdReg = one_hundred_closure;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR_ID]); // continue with VarId
}
StgWord main_x_info[] = {main_x_entry};

// var_x = {x} \n {} -> Var {x}
CodeLabel main_var_x_entry() {
  PRINT_FUNCTION_NAME();
  StgWord x = Node[1];
  ExprReg1 = x;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_VAR]); // continue with Var
}
StgWord main_var_x_info[] = {main_var_x_entry};

// lit_one = {} \n {} -> Lit {one}
CodeLabel main_lit_one_entry() {
  PRINT_FUNCTION_NAME();
  ExprReg1 = one_closure;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_LIT]); // continue with Lit
}
StgWord main_lit_one_info[] = {main_lit_one_entry};

// add = {var_x,lit_one} \n {} -> Add {var_x,lit_one}
CodeLabel main_add_entry() {
  PRINT_FUNCTION_NAME();
  StgWord var_x = Node[1];
  StgWord lit_one = Node[2];
  ExprReg1 = var_x;
  ExprReg2 = lit_one;
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_ADD]); // continue with Add
}
StgWord main_add_info[] = {main_add_entry};

// e = {add} \u {} -> sop {add,two}
CodeLabel main_e_entry() {
  PRINT_FUNCTION_NAME();
  StgWord add = Node[1];
  push_a(two_closure, NAME_OF(two_closure));
  push_a(add, NAME_OF(add));
  JUMP(sop_direct); // static
}
StgWord main_e_info[] = {main_e_entry};

// Int# iI -> case iI {}
CodeLabel main_valueOf_return_Int() {
  PRINT_FUNCTION_NAME();
  int iI = IntReg;
  if (iI == 100) {
    IntReg = 8;
  } else {
    IntReg = 0;
  }
  pop_a(); // pop varid_i
  RetVecReg = pop_b();
  JUMP(RetVecReg[RET_INT]); // continue with Int
}
StgWord main_valueOf_return_vec2[] = {main_valueOf_return_Int};

// VarId {i} -> case i {}
CodeLabel main_valueOf_return_VarId() {
  PRINT_FUNCTION_NAME();
  push_b(main_valueOf_return_vec2, NAME_OF(main_valueOf_return_vec2));
  Node = VarIdReg;
  ENTER(Node); // enter i
}
StgWord main_valueOf_return_vec1[] = {main_valueOf_return_VarId};

// valueOf = {} \n {varid_i} -> case varid_i {}
CodeLabel main_valueOf_entry() {
  PRINT_FUNCTION_NAME();
  push_b(main_valueOf_return_vec1, NAME_OF(main_valueOf_return_vec1));
  Node = arg(0);
  ENTER(Node); // enter varid_i
}
StgWord main_valueOf_info[] = {main_valueOf_entry};

CodeLabel main_direct() {
  PRINT_FUNCTION_NAME();
  // fill closure x = {} \n {} -> VarId {one_hundred}
  StgWord *x_closure = allocate(1);
  x_closure[0] = main_x_info;
  // fill closure var_x = {x} \n {} -> Var {x}
  StgWord *var_x_closure = allocate(2);
  var_x_closure[0] = main_var_x_info;
  var_x_closure[1] = x_closure;
  // fill closure lit_one = {} \n {} -> Lit {one}
  StgWord *lit_one_closure = allocate(1);
  lit_one_closure[0] = main_lit_one_info;
  // fill closure add = {var_x,lit_one} \n {} -> Add {var_x,lit_one}
  StgWord *add_closure = allocate(3);
  add_closure[0] = main_add_info;
  add_closure[1] = var_x_closure;
  add_closure[2] = lit_one_closure;
  // fill closure e = {add} \u {} -> sop {add,two}
  StgWord *e_closure = allocate(2);
  e_closure[0] = main_e_info;
  e_closure[1] = add_closure;
  // fill valueOf = {} \n {varid_i} -> case varid_i {}
  StgWord *valueOf_closure = allocate(1);
  valueOf_closure[0] = main_valueOf_info;
  // call eval {valueOf,e}
  push_a(e_closure, NAME_OF(e_closure));
  push_a(valueOf_closure, NAME_OF(valueOf_closure));
  JUMP(eval_direct); // static
}

CodeLabel main_direct_debug() {
  PRINT_FUNCTION_NAME();
  JUMP(one_direct); // static
}
CodeLabel main_entry() {
  PRINT_FUNCTION_NAME();
  JUMP(main_direct);
}
StgWord main_info[] = {main_entry};

int main() {
  push_b(return_int_return_vec, NAME_OF(return_int_return_vec));
  CodeLabel cont = (CodeLabel)&main_entry;
  while (1) {
    cont = (*cont)();
  }
  return 0;
}
