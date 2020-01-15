#include "ir/oir.h"


DEFINE_LIST(release_BasicBlock, BasicBlock*, BBList)

OIR* new_OIR() {
  OIR* ir = calloc(1, sizeof(OIR));
  ir->blocks = new_BBList();
  ir->instructions = new_InstList();
  return ir;
}

void release_OIR(OIR* ir) {
  if (ir == NULL) {
    return;
  }
  release_BBList(ir->blocks);
  release_InstList(ir->instructions);
  free(ir);
}

void detach_BasicBlock(OIR* ir, BasicBlock* b) {
  // detach a block from IR and release it safely.
  // - check entry/exit
  // - remove from succs/preds
  // - remove from blocks

  assert(ir->entry != b);
  assert(ir->exit != b);

  {
    BBRefListIterator* it = front_BBRefList(b->succs);
    while (!is_nil_BBRefListIterator(it)) {
      BasicBlock* suc = data_BBRefListIterator(it);
      erase_one_BBRefList(suc->preds, b);
      it = next_BBRefListIterator(it);
    }
  }
  {
    BBRefListIterator* it = front_BBRefList(b->preds);
    while (!is_nil_BBRefListIterator(it)) {
      BasicBlock* pre = data_BBRefListIterator(it);
      erase_one_BBRefList(pre->succs, b);
      it = next_BBRefListIterator(it);
    }
  }

  erase_one_BBList(ir->blocks, b);
}
