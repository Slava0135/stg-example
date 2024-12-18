#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define JUMP(code_label) return (CodeLabel)&code_label
#define ENTER(node) JUMP( (*((CodeLabel **)node[0]))[0] )
// StgWord*  node
// StgWord   node[0]

typedef void* (*CodeLabel)() ;
typedef void* StgWord ;

StgWord Sp[10000];
StgWord *SpB = Sp;
StgWord **SpA = &Sp[10000];

StgWord RetData1;
StgWord RetData2;

StgWord *RetVecReg;

StgWord *Node;  // current closure object

// +
CodeLabel plus_ret_MkInt2() {
  // k2 = RetData1;
  int k1 = (int)SpB[0]; SpB = SpB - 1; // pop k1 local variable
  int k3 = k1 + (int)RetData2; // k1 + k2
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[0] );              // continue with MkInt
}
StgWord plus_ret_vec2[] = { plus_ret_MkInt2 };
CodeLabel plus_ret_MkInt1() {
  // k1 = RetData1;
  // push case alternatives on return stack
  SpB = SpB + 1; SpB[0] = (StgWord)RetData1; // push k on stack B
  SpB = SpB + 1; SpB[0] = plus_ret_vec2;
  Node = SpA[1]; ENTER( Node ); // enter m
}
StgWord plus_ret_vec1[] = { plus_ret_MkInt1 };
CodeLabel plus_direct() {
  printf("[debug] plus_direct\n");
  // push case alternatives on return stack
  SpB = SpB + 1; SpB[0] = plus_ret_vec1;
  Node = SpA[0]; ENTER( Node ); // enter n
}
CodeLabel plus_entry() {
  printf("[debug] plus_entry\n");
  JUMP( plus_direct );
}
StgWord plus_info[] = {plus_entry};
StgWord plus_closure[] = {&plus_info};

// constNil = {} \n {ys} → Nil {}
CodeLabel constNil_direct() {
  printf("[debug] constNil_direct\n");
  SpA = SpA + 1; // pop arguments {ys}
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[0] );              // continue with Nil
}
CodeLabel constNil_entry() {
  printf("[debug] constNil_entry\n");
  JUMP( constNil_direct );
}
StgWord constNil_info[] = {constNil_entry};
StgWord constNil_closure[] = {&constNil_info};

// zipWith = {} \n {f} → …
CodeLabel zipWith_go_a_fxz_entry() {
  printf("[debug] zipWith_go_a_fxz_entry\n");
  SpA[-1] = Node[3]; SpA = SpA - 1; // push z on stack A
  SpA[-1] = Node[2]; SpA = SpA - 1; // push x on stack A
  Node = Node[1]; ENTER( Node ); // enter f
}
StgWord zipWith_go_a_fxz_info[] = {zipWith_go_a_fxz_entry};

CodeLabel zipWith_go_a_goxszs_entry() {
  printf("[debug] zipWith_go_a_goxszs_entry\n");
  SpA[-1] = Node[3]; SpA = SpA - 1; // push zs on stack A
  SpA[-1] = Node[2]; SpA = SpA - 1; // push xs' on stack A
  Node = Node[1]; ENTER( Node ); // enter go
}
StgWord zipWith_go_a_goxszs_info[] = {zipWith_go_a_goxszs_entry};

CodeLabel zipWith_go_a_ret_Nil() {
  SpA = SpA + 4; // pop local variables {go, f, x, xs'}
  SpA = SpA + 1; // pop arguments {ys}
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[0] );              // continue with Nil
}
CodeLabel zipWith_go_a_ret_Cons() {
  // z = RetData1;
  // zs = RetData2;
  StgWord *fxz_closure = malloc(sizeof(StgWord) * 4);
  fxz_closure[0] = &zipWith_go_a_fxz_info;
  fxz_closure[1] = SpA[2]; // f
  fxz_closure[2] = SpA[1]; // x
  fxz_closure[3] = RetData1; // z

  StgWord *goxszs_closure = malloc(sizeof(StgWord) * 4);
  goxszs_closure[0] = &zipWith_go_a_goxszs_info;
  goxszs_closure[1] = SpA[3]; // go
  goxszs_closure[2] = SpA[0]; // xs'
  goxszs_closure[3] = RetData2; // zs

  SpA = SpA + 4; // pop local variables {go, f, x, xs'}
  SpA = SpA + 1; // pop arguments {ys}

  RetData1 = fxz_closure;
  RetData2 = goxszs_closure;
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[1] );              // continue with Cons
}
StgWord zipWith_go_a_ret_vec[] = {zipWith_go_a_ret_Nil, zipWith_go_a_ret_Cons};

CodeLabel zipWith_go_a_entry() {
  printf("[debug] zipWith_go_a_entry\n");
  // ys = SpA[0];
  SpA[-1] = Node[1]; SpA = SpA - 1; // push go  on stack A
  SpA[-1] = Node[2]; SpA = SpA - 1; // push f   on stack A
  SpA[-1] = Node[3]; SpA = SpA - 1; // push x   on stack A
  SpA[-1] = Node[4]; SpA = SpA - 1; // push xs' on stack A
  SpB = SpB + 1; SpB[0] = zipWith_go_a_ret_vec;
  // ys = SpA[4];
  Node = SpA[4]; ENTER( Node ); // enter ys
}
StgWord zipWith_go_a_info[] = {zipWith_go_a_entry};

CodeLabel zipWith_go_ret_Nil() {
  SpA = SpA + 2; // pop local variables {go, f}
  SpA = SpA + 1; // pop arguments {xs}
  // Node = constNil_closure; ENTER( Node );
  JUMP( constNil_direct );
}
CodeLabel zipWith_go_ret_Cons() {
  // x = RetData1;
  // xs' = RetData2;
  StgWord *a_closure = malloc(sizeof(StgWord) * 5);
  a_closure[0] = &zipWith_go_a_info;
  a_closure[1] = SpA[1]; // go
  a_closure[2] = SpA[0]; // f
  a_closure[3] = RetData1; // x
  a_closure[4] = RetData2; // xs'
  Node = a_closure; ENTER( Node );
}
StgWord zipWith_go_ret_vec[] = {zipWith_go_ret_Nil, zipWith_go_ret_Cons};

CodeLabel zipWith_go_entry() {
  printf("[debug] zipWith_go_entry\n");
  // push case alternatives on return stack
  // xs = SpA[0];
  SpA[-1] = Node[1]; SpA = SpA - 1; // push go on stack A
  SpA[-1] = Node[2]; SpA = SpA - 1; // push f  on stack A
  SpB = SpB + 1; SpB[0] = zipWith_go_ret_vec;
  // xs = SpA[2];
  Node = SpA[2]; ENTER( Node ); // enter xs
}
StgWord zipWith_go_info[] = {zipWith_go_entry};

CodeLabel zipWith_direct() {
  StgWord *go_closure;
  printf("[debug] zipWith_direct\n");

  go_closure = malloc(sizeof(StgWord) * 3);
  go_closure[0] = &zipWith_go_info;
  go_closure[1] = go_closure;
  go_closure[2] = SpA[0]; // f
  SpA = SpA + 1;  // pop arguments {f}
  Node = go_closure; ENTER( Node );
}

// id = {} \u {n} → n
CodeLabel id_direct() {
  printf("[debug] id_direct\n");
  Node = SpA[0]; SpA = SpA + 1; ENTER( Node );
}
CodeLabel id_entry() {
  printf("[debug] id_entry\n");
  JUMP( id_direct );
}
const StgWord id_info[] = {id_entry};
StgWord id_closure[] = {&id_info};

// head = {} \n {xs} →
//   case xs {} of
//     Nil {} → MkInt {0#}
//     Cons {z, zs} → z
CodeLabel head_ret_nil() {
  SpA = SpA + 1;  // pop arguments {xs}
  RetData1 = (StgWord)0;  // put 0# in the return register
  SpB = SpB - 1; RetVecReg = SpB[1];
  JUMP( *RetVecReg[0] );  // continue with MkInt
}
CodeLabel head_ret_cons() {
  SpA = SpA + 1;  // pop arguments {xs}
  Node = RetData1; ENTER( Node ); // enter z
}
StgWord head_ret_vec[] = {head_ret_nil, head_ret_cons};

CodeLabel head_direct() {
  printf("[debug] head_direct\n");
  // push case alternatives on return stack
  SpB = SpB + 1; SpB[0] = head_ret_vec;
  Node = SpA[0]; ENTER( Node ); // enter xs
}
CodeLabel head_entry() {
  printf("[debug] head_entry\n");
  JUMP( head_direct );
}
const StgWord head_info[] = {head_entry};
StgWord head_closure[] = {&head_info};

// std_MkInt = {x} \u {} → MkInt {x}
CodeLabel std_MkInt_direct() {
  printf("[debug] std_MkInt_direct\n");
  RetData1 = Node[1];
  SpB = SpB - 1; RetVecReg = SpB[1];
  JUMP( *RetVecReg[0] ); // continue for MkInt
}
CodeLabel std_MkInt_entry() {
  printf("[debug] std_MkInt_entry\n");
  JUMP( std_MkInt_direct );
}
const StgWord std_MkInt_info[] = {std_MkInt_entry};
// StgWord std_MkInt_closure[] = {&std_MkInt_info, x};

// zero = {} \u {} → MkInt {0#}
StgWord zero_closure[] = {&std_MkInt_info, (StgWord)0};
// one = {} \u {} → MkInt {1#}
StgWord one_closure[] = {&std_MkInt_info, (StgWord)1};
// two = {} \u {} → MkInt {2#}
StgWord two_closure[] = {&std_MkInt_info, (StgWord)2};
// three = {} \u {} → MkInt {3#}
StgWord three_closure[] = {&std_MkInt_info, (StgWord)3};

// std_Cons = {x, xs} \n {} → Cons {x, xs}
CodeLabel std_Cons_entry();
const StgWord std_Cons_info[] = {std_Cons_entry};
// StgWord std_Cons_closure[] = {&std_Cons_info, x, xs};
CodeLabel std_Cons_direct() {
  printf("[debug] std_Cons_direct\n");
  RetData1 = Node[1];
  RetData2 = Node[2];
  SpB = SpB - 1; RetVecReg = SpB[1];
  JUMP( *RetVecReg[1] ); // continue for Cons
}
CodeLabel std_Cons_entry() {
  printf("[debug] std_Cons_entry\n");
  JUMP( std_Cons_direct );
}

// threes = {} \n {} → Cons {three, threes}
StgWord threes_closure[] =
  {&std_Cons_info, three_closure, threes_closure};

// drop = {} \n {n, xs} → …
CodeLabel drop_direct();
CodeLabel drop_ret_Nil2() {
  printf("[debug] drop_ret_Nil2\n");
  SpA = SpA + 2;  // pop arguments {n, xs}
  SpB = SpB - 1;  // pop local variable k
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[0] );              // continue with Nil
}
CodeLabel drop_ret_Cons2() {
  StgWord *n_prime_closure = malloc(sizeof(StgWord) * 2);
  n_prime_closure[0] = &std_MkInt_info;
  printf("[debug] drop_ret_Cons2\n");
  // z = RetData1
  // zs = RetData2
  SpB = SpB - 1; int k = (int)SpB[1]; // pop local variable k
  int k_prime = (k - 1);
  switch (k_prime) {
    default:
      n_prime_closure[1] = (StgWord)k_prime;
      SpA = SpA + 2;  // pop arguments {n, xs}
      SpA[-1] = RetData2; SpA = SpA - 1; // push zs on stack A
      SpA[-1] = n_prime_closure; SpA = SpA - 1; // push n' on stack A
      // Node = drop_closure; ENTER ( Node ); // enter drop
      JUMP( drop_direct );
      break;
  }
}
StgWord drop_ret_vec2[] = { drop_ret_Nil2 /*, drop_upd_Nil2 */, drop_ret_Cons2 /*, drop_upd_Cons2 */};

CodeLabel drop_ret_MkInt1() {
  printf("[debug] drop_ret_MkInt1\n");
  int k = (int)RetData1;
  switch (k) {
    case 0:
      Node = SpA[1];  // Node = xs
      SpA = SpA + 2;  // pop arguments {n, xs}
      ENTER( Node );
      break;
    default:
      // push case alternatives on return stack
      SpB = SpB + 1; SpB[0] = (StgWord)k;
      SpB = SpB + 1; SpB[0] = drop_ret_vec2;
      Node = SpA[1]; ENTER( Node ); // enter xs
      break;
  }
}
StgWord drop_ret_vec1[] = { drop_ret_MkInt1 /*, drop_upd_MkInt1 */ };

CodeLabel drop_direct() {
  printf("[debug] drop_direct\n");
  // push case alternatives on return stack
  SpB = SpB + 1; SpB[0] = drop_ret_vec1;
  Node = SpA[0]; ENTER( Node ); // enter n
}

// fibs = {} \u {} → …
CodeLabel fibs_entry();
StgWord fibs_info[] = {fibs_entry};
StgWord *fibs_closure[] = {&fibs_info};

CodeLabel fibs_drop1fibs_entry() {
  printf("[debug] fibs_drop1fibs_entry\n");
  SpA[-1] = fibs_closure; SpA = SpA - 1; // push fibs on stack A
  SpA[-1] = one_closure; SpA = SpA - 1; // push one on stack A
  JUMP( drop_direct );
}
StgWord fibs_drop1fibs_info[] = {fibs_drop1fibs_entry};

CodeLabel fibs_fibs2_entry() {
  printf("[debug] fibs_fibs2_entry\n");
  SpA[-1] = Node[1];      SpA = SpA - 1; // push drop1fibs on stack A
  SpA[-1] = fibs_closure; SpA = SpA - 1; // push fibs on stack A
  SpA[-1] = plus_closure; SpA = SpA - 1; // push + on stack A
  JUMP( zipWith_direct );
}
StgWord fibs_fibs2_info[] = {fibs_fibs2_entry};

CodeLabel fibs_direct() {
  printf("[debug] fibs_direct\n");
  StgWord *fibs_drop1fibs_closure = malloc(sizeof(StgWord) * 1);
  fibs_drop1fibs_closure[0] = &fibs_drop1fibs_info;

  StgWord *fibs_fibs2_closure = malloc(sizeof(StgWord) * 2);
  fibs_fibs2_closure[0] = &fibs_fibs2_info;
  fibs_fibs2_closure[1] = fibs_drop1fibs_closure;

  StgWord *fibs_fibs1_closure = malloc(sizeof(StgWord) * 3);
  fibs_fibs1_closure[0] = &std_Cons_info;
  fibs_fibs1_closure[1] = one_closure;
  fibs_fibs1_closure[2] = fibs_fibs2_closure;

  RetData1 = zero_closure; RetData2 = fibs_fibs1_closure;
  SpB = SpB - 1; RetVecReg = SpB[1];  // pop return vector
  JUMP( *RetVecReg[1] );              // continue with Cons
}
CodeLabel fibs_entry() {
  printf("[debug] fibs_entry\n");
  JUMP( fibs_direct );
}

// fib = {} \n {n} →
//   let dropnfibs = {n} \n {} → drop {n, fibs}
//    in head {dropnfibs}
CodeLabel dropnfibs_entry() {
  printf("[debug] dropnfibs_entry\n");
  // n = Node[1]
  SpA[-1] = fibs_closure; SpA = SpA - 1; // push fibs on stack A
  SpA[-1] = Node[1]; SpA = SpA - 1; // push n on stack A
  // Node = drop_closure; ENTER( drop_closure );
  JUMP( drop_direct );
}
StgWord dropnfibs_info[] = {dropnfibs_entry};

CodeLabel fib_direct() {
  StgWord *dropnfibs_closure = malloc(sizeof(StgWord) * 2);
  dropnfibs_closure[0] = &dropnfibs_info;
  printf("[debug] fib_direct\n");
  dropnfibs_closure[1] = SpA[0];
  SpA = SpA + 1;  // pop arguments {n}
  SpA[-1] = dropnfibs_closure; SpA = SpA - 1; // push dropnfibs on stack A
  // Node = head_closure; ENTER( head_closure );
  JUMP( head_direct );
}

// main = {} \u {} → fib {three}
CodeLabel main_direct() {
  printf("[debug] main_direct\n");
  SpA[-1] = two_closure; SpA = SpA - 1; // push three on stack A
  // Node = &fib_closure; ENTER( fib_closure );
  JUMP( fib_direct );
}
CodeLabel main_entry() {
  printf("[debug] main_entry\n");
  JUMP( main_direct );
}
StgWord main_info[] = {main_entry};
StgWord *main_closure[] = {&main_info};

// debug

CodeLabel final_ret_MkInt() {
  printf("[debug] final_ret_MkInt\n");
  int n = (int)RetData1;
  printf("Result = %d\n", n);
  exit(0);
}
StgWord final_ret_vec[] = { final_ret_MkInt /*, upd_mkInt */ };

int main() {
  printf("Running fib-stg...\n");

  SpB = SpB + 1; SpB[0] = final_ret_vec;
  CodeLabel cont = (CodeLabel)&main_entry;
  while (TRUE) {
    cont = (*cont)();
  }
  return 0;
}