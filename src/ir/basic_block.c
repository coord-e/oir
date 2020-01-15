#include "ir/basic_block.h"

#include "util.h"


DEFINE_LIST(release_void, BasicBlock*, BBRefList)
DEFINE_LIST(release_Inst, Inst*, InstList)
DEFINE_RANGE(Inst*, InstList, InstRange)

BasicBlock* new_BasicBlock(unsigned id) {
  BasicBlock* block = calloc(1, sizeof(BasicBlock));
  block->id = id;
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
