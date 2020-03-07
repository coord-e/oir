#ifndef OIR_IR_INST_H
#define OIR_IR_INST_H

#include <stdio.h>

#include "container/bit_set.h"
#include "container/vector.h"
#include "ir/reg.h"

DECLARE_VECTOR(Reg*, RegVec)
typedef struct BasicBlock BasicBlock;

typedef enum {
  IR_MOV,
  IR_IMM,
  IR_ADD,
  IR_EQUAL,
  IR_LABEL,
  IR_BRANCH,
  IR_JUMP,
  IR_RETURN,
} InstKind;

typedef struct {
  InstKind kind;
  unsigned id;

  long imm;  // for IR_IMM

  char* label_name;   // for IR_LABEL
  BasicBlock* label;  // ref; for IR_LABEL

  char* jump_name;   // for IR_JUMP
  BasicBlock* jump;  // ref; for IR_JUMP

  char* then_name;    // for IR_BRANCH
  char* else_name;    // for IR_BRANCH
  BasicBlock* then_;  // ref; for IR_BRANCH
  BasicBlock* else_;  // ref; for IR_BRANCH

  Reg* rd;     // output register (null if unused)
  RegVec* rs;  // input registers (won't be null)

  unsigned expression_id;

  BitSet* live_in;
  BitSet* live_out;
  BitSet* reach_in;
  BitSet* reach_out;
  BitSet* available_in;
  BitSet* available_out;
} Inst;

Inst* new_Inst(unsigned id, InstKind);
void release_Inst(Inst*);
void print_Inst(FILE*, Inst*);
void print_json_Inst(FILE*, Inst*);

bool is_expression(Inst*);
bool compare_expressions(Inst*, Inst*);

#endif
