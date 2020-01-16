#ifndef OIR_IR_BASIC_BLOCK_H
#define OIR_IR_BASIC_BLOCK_H

#include <stdio.h>

#include "container/bit_set.h"
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

  BitSet* live_in;
  BitSet* live_out;
  BitSet* live_gen;
  BitSet* live_kill;
  BitSet* reach_in;
  BitSet* reach_out;
  BitSet* reach_gen;
  BitSet* reach_kill;
};

BasicBlock* new_BasicBlock(unsigned id);
void release_BasicBlock(BasicBlock*);
void print_BasicBlock(FILE*, BasicBlock*);
void print_graph_BasicBlock(FILE*, BasicBlock*);

void connect_BasicBlock(BasicBlock* from, BasicBlock* to);
void disconnect_BasicBlock(BasicBlock* from, BasicBlock* to);

#endif
