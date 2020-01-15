#include "ir/basic_block.h"

#include "util.h"

DEFINE_LIST(release_void, BasicBlock*, BBRefList)
DEFINE_LIST(release_Inst, Inst*, InstList)
DEFINE_RANGE(Inst*, InstList, InstRange)

BasicBlock* new_BasicBlock(unsigned id) {
  BasicBlock* block   = calloc(1, sizeof(BasicBlock));
  block->id           = id;
  block->instructions = new_unchecked_InstRange(NULL, NULL);
  block->succs        = new_BBRefList();
  block->preds        = new_BBRefList();
  return block;
}

void release_BasicBlock(BasicBlock* block) {
  if (block == NULL) {
    return;
  }
  release_InstRange(block->instructions);
  release_BBRefList(block->succs);
  release_BBRefList(block->preds);
  free(block);
}

void connect_BasicBlock(BasicBlock* from, BasicBlock* to) {
  push_back_BBRefList(from->succs, to);
  push_back_BBRefList(to->preds, from);
}

void disconnect_BasicBlock(BasicBlock* from, BasicBlock* to) {
  erase_one_BBRefList(from->succs, to);
  erase_one_BBRefList(to->preds, from);
}

static void print_block_set(FILE* p, BBRefList* bbs) {
  fprintf(p, "{");
  for (BBRefListIterator* it = front_BBRefList(bbs); !is_nil_BBRefListIterator(it);
       it                    = next_BBRefListIterator(it)) {
    BasicBlock* block = data_BBRefListIterator(it);
    fprintf(p, "%d", block->id);
    if (!is_nil_BBRefListIterator(next_BBRefListIterator(it))) {
      fprintf(p, ", ");
    }
  }
  fprintf(p, "}");
}

void print_BasicBlock(FILE* p, BasicBlock* block) {
  fprintf(p, "# BB%d succs=", block->id);
  print_block_set(p, block->succs);
  fprintf(p, " preds=");
  print_block_set(p, block->preds);
  fprintf(p, "\n");

  for (InstRangeIterator* it = front_InstRange(block->instructions); !is_nil_InstRangeIterator(it);
       it                    = next_InstRangeIterator(it)) {
    Inst* inst = data_InstRangeIterator(it);
    print_Inst(p, inst);
    fprintf(p, "\n");
  }
}
