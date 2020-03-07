#include "ir/basic_block.h"

#include "util/convention.h"
#include "util/dummy.h"
#include "util/print_json.h"

DEFINE_LIST(dummy_release, BasicBlock*, BBRefList)
static unsigned get_id(Inst* inst) {
  return inst->id;
}
DEFINE_SELF_INDEXED_LIST(get_id, release_Inst, Inst*, InstList)
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
  release_BitSet(block->live_in);
  release_BitSet(block->live_out);
  release_BitSet(block->live_gen);
  release_BitSet(block->live_kill);
  release_BitSet(block->reach_in);
  release_BitSet(block->reach_out);
  release_BitSet(block->reach_gen);
  release_BitSet(block->reach_kill);
  release_BitSet(block->available_in);
  release_BitSet(block->available_out);
  release_BitSet(block->available_gen);
  release_BitSet(block->available_kill);
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
  FOR_EACH (BasicBlock*, block, BBRefList, bbs) {
    fprintf(p, "%s%d%s", prefix, block->id, suffix);
    if (!is_nil_BBRefListIterator(next_BBRefListIterator(it_block))) {
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

  FOR_EACH (Inst*, inst, InstRange, block->instructions) {
    print_Inst(p, inst);
    fprintf(p, "\n");
  }
}

void print_graph_BasicBlock(FILE* p, BasicBlock* block) {
  fprintf(p, "block_%d [shape = record, fontname = monospace, label = \"{<init>", block->id);
  FOR_EACH (Inst*, inst, InstRange, block->instructions) {
    bool is_last = is_nil_InstRangeIterator(next_InstRangeIterator(it_inst));
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

static void print_json_block_set(FILE* f, BBRefList* bbs) {
  print_block_set(f, "", "", bbs);
}

static void print_json_insts(FILE* f, InstRange* insts) {
  FOR_EACH (Inst*, inst, InstRange, insts) {
    print_json_Inst(f, inst);
    if (!is_nil_InstRangeIterator(next_InstRangeIterator(it_inst))) {
      fprintf(f, ", ");
    }
  }
}

void print_json_BasicBlock(FILE* f, BasicBlock* block) {
  JSON_PRINT_START(f);

  JSON_PRINT(f, "id", block->id, print_unsigned);
  JSON_PRINT_ARRAY(f, "succs", block->succs, print_json_block_set);
  JSON_PRINT_ARRAY(f, "preds", block->preds, print_json_block_set);

  JSON_PRINT_ARRAY_MAYBE(f, "live_in", block->live_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "live_out", block->live_out, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "live_gen", block->live_gen, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "live_kill", block->live_kill, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_in", block->reach_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_out", block->reach_out, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_gen", block->reach_gen, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_kill", block->reach_kill, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_in", block->available_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_out", block->available_out, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_gen", block->available_gen, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_kill", block->available_kill, print_BitSet);

  JSON_PRINT_ARRAY_LAST(f, "instructions", block->instructions, print_json_insts);

  JSON_PRINT_END(f);
}
