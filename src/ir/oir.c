#include "ir/oir.h"

DEFINE_LIST(release_BasicBlock, BasicBlock*, BBList)
DEFINE_VECTOR(release_BitSet, BitSet*, BSVec)

OIR* new_OIR() {
  OIR* ir          = calloc(1, sizeof(OIR));
  ir->blocks       = new_BBList();
  ir->instructions = new_InstList(32);
  return ir;
}

void release_OIR(OIR* ir) {
  if (ir == NULL) {
    return;
  }
  release_BBList(ir->blocks);
  release_InstList(ir->instructions);
  release_BSVec(ir->definitions);
  release_BSVec(ir->kill_expressions);
  free(ir);
}

void detach_BasicBlock(OIR* ir, BasicBlock* b) {
  // detach a block from IR and release it safely.
  // - check entry/exit
  // - remove from succs/preds
  // - remove from blocks

  assert(ir->entry != b);
  assert(ir->exit != b);

  FOR_EACH (BasicBlock*, suc, BBRefList, b->succs) { erase_one_BBRefList(suc->preds, b); }
  FOR_EACH (BasicBlock*, pre, BBRefList, b->preds) { erase_one_BBRefList(pre->succs, b); }

  erase_one_BBList(ir->blocks, b);
}

void print_OIR(FILE* p, OIR* ir) {
  fprintf(p, "# OIR entry=%d, exit=%d\n", ir->entry->id, ir->exit->id);
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) { print_BasicBlock(p, block); }
}

void print_graph_OIR(FILE* p, OIR* ir) {
  fprintf(p, "digraph CFG {\n");
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) { print_graph_BasicBlock(p, block); }
  fprintf(p, "}\n");
}
