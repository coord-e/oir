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

static void print_block_set(FILE* p, const char* prefix, const char* suffix, BBRefList* bbs) {
  for (BBRefListIterator* it = front_BBRefList(bbs); !is_nil_BBRefListIterator(it);
       it                    = next_BBRefListIterator(it)) {
    BasicBlock* block = data_BBRefListIterator(it);
    fprintf(p, "%s%d%s", prefix, block->id, suffix);
    if (!is_nil_BBRefListIterator(next_BBRefListIterator(it))) {
      fprintf(p, ", ");
    }
  }
}

void print_BasicBlock(FILE* p, BasicBlock* block) {
  fprintf(p, "# BB%d succs={", block->id);
  print_block_set(p, "", "", block->succs);
  fprintf(p, "} preds={");
  print_block_set(p, "", "", block->preds);
  fprintf(p, "}\n");

  for (InstRangeIterator* it = front_InstRange(block->instructions); !is_nil_InstRangeIterator(it);
       it                    = next_InstRangeIterator(it)) {
    Inst* inst = data_InstRangeIterator(it);
    print_Inst(p, inst);
    fprintf(p, "\n");
  }
}

void print_graph_BasicBlock(FILE* p, BasicBlock* block) {
  fprintf(p, "block_%d [shape = record, fontname = monospace, label = \"{<init>", block->id);
  for (InstRangeIterator* it = front_InstRange(block->instructions); !is_nil_InstRangeIterator(it);
       it                    = next_InstRangeIterator(it)) {
    Inst* inst   = data_InstRangeIterator(it);
    bool is_last = is_nil_InstRangeIterator(next_InstRangeIterator(it));
    if (is_last) {
      fprintf(p, "<last>");
    }
    print_Inst(p, inst);
    fprintf(p, "\\l");
    if (!is_last) {
      fprintf(p, "|");
    }
  }
  fprintf(p, "}\"]\n");

  if (!is_empty_BBRefList(block->succs)) {
    fprintf(p, "block_%d:last:s -> ", block->id);
    print_block_set(p, "block_", ":init:n", block->succs);
    fprintf(p, "\n");
  }
}
