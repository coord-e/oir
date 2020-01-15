#ifndef OIR_IR_INST_H
#define OIR_IR_INST_H

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

  long imm;           // for IR_IMM

  BasicBlock* label;  // ref; for IR_LABEL

  BasicBlock* jump;   // ref; for IR_JUMP

  BasicBlock* then_;  // ref; for IR_BRANCH
  BasicBlock* else_;  // ref; for IR_BRANCH

  Reg* rd;            // output register (null if unused)
  RegVec* rs;         // input registers (won't be null)
} Inst;

Inst* new_Inst(unsigned id, InstKind);
void release_Inst(Inst*);

#endif
