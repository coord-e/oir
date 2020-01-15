#ifndef OIR_IR_BASIC_BLOCK_H
#define OIR_IR_BASIC_BLOCK_H

#include <stdio.h>

#include "container/list.h"
#include "container/range.h"
#include "ir/inst.h"

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
void print_BasicBlock(FILE*, BasicBlock*);
void print_graph_BasicBlock(FILE*, BasicBlock*);

void connect_BasicBlock(BasicBlock* from, BasicBlock* to);
void disconnect_BasicBlock(BasicBlock* from, BasicBlock* to);

#endif
