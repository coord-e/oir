#ifndef OIR_IR_H
#define OIR_IR_H

#include "list.h"
#include "range.h"
#include "vector.h"


typedef enum {
  REG_VIRT,
  REG_REAL,
} RegKind;

typedef struct {
  RegKind kind;
  unsigned virtual;
  unsigned real;
} Reg;

Reg* new_virtual_Reg(unsigned virtual);
Reg* new_real_Reg(unsigned real);
Reg* copy_Reg(Reg*);
void release_Reg(Reg*);


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


DECLARE_LIST(BasicBlock*, BBRefList)
DECLARE_LIST(Inst*, InstList)
DECLARE_RANGE(Inst*, InstList, InstRange)

// `BasicBlock` forms a control flow graph
struct BasicBlock {
  unsigned id;

  // reference to the specific range in `instructions` in `Function`
  InstRange* instructions;

  BBRefList* succs;
  BBRefList* preds;
};

BasicBlock* new_BasicBlock(unsigned id);
void release_BasicBlock(BasicBlock*);

void connect_BasicBlock(BasicBlock* from, BasicBlock* to);
void disconnect_BasicBlock(BasicBlock* from, BasicBlock* to);


DECLARE_LIST(BasicBlock*, BBList)

typedef struct {
  BBList* blocks;
  InstList* instructions;

  unsigned block_count;
  unsigned inst_count;

  BasicBlock* entry;  // ref
  BasicBlock* exit;   // ref
} OIR;

OIR* new_OIR();
void release_OIR(OIR*);

void detach_BasicBlock(OIR*, BasicBlock*);

#endif
