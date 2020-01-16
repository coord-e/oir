#ifndef OIR_IR_OIR_H
#define OIR_IR_OIR_H

#include <stdio.h>

#include "container/bit_set.h"
#include "container/list.h"
#include "container/vector.h"
#include "ir/basic_block.h"

DECLARE_LIST(BasicBlock*, BBList)
DECLARE_VECTOR(BitSet*, BSVec)

typedef struct {
  BBList* blocks;
  InstList* instructions;

  unsigned block_count;
  unsigned inst_count;
  unsigned reg_count;
  unsigned expr_count;

  BasicBlock* entry;  // ref
  BasicBlock* exit;   // ref

  BSVec* definitions;
  BSVec* kill_expressions;
} OIR;

OIR* new_OIR();
void release_OIR(OIR*);
void print_OIR(FILE*, OIR*);
void print_graph_OIR(FILE*, OIR*);

void detach_BasicBlock(OIR*, BasicBlock*);

#endif
