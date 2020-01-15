#ifndef OIR_IR_OIR_H
#define OIR_IR_OIR_H

#include "container/list.h"
#include "ir/basic_block.h"

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
