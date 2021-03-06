#include "ir/oir.h"

#include "util/convention.h"
#include "util/dummy.h"
#include "util/print_json.h"

DEFINE_LIST(release_BasicBlock, BasicBlock*, BBList)
DEFINE_VECTOR(release_BitSet, BitSet*, BSVec)
DEFINE_VECTOR(dummy_release, Inst*, InstRefVec)

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

static void print_json_blocks(FILE* f, BBList* blocks) {
  FOR_EACH (BasicBlock*, block, BBList, blocks) {
    print_json_BasicBlock(f, block);
    if (!is_nil_BBListIterator(next_BBListIterator(it_block))) {
      fprintf(f, ", ");
    }
  }
}

static void print_json_BSVec(FILE* f, BSVec* bsv) {
  fprintf(f, "{");
  FOR_EACH_VEC (BitSet*, bs, BSVec, bsv) {
    if (i_bs != 0) {
      fputs(",", f);
    }
    fprintf(f, "\"%d\": [", i_bs);
    print_BitSet(f, bs);
    fprintf(f, "]");
  }
  fprintf(f, "}");
}

void print_json_OIR(FILE* f, OIR* ir) {
  JSON_PRINT_START(f);

  JSON_PRINT(f, "entry", ir->entry->id, print_unsigned);
  JSON_PRINT(f, "exit", ir->exit->id, print_unsigned);

  JSON_PRINT_ARRAY(f, "blocks", ir->blocks, print_json_blocks);
  // JSON_PRINT_ARRAY(f, "instructions", ir->instructions, print_json_insts);

  JSON_PRINT_MAYBE(f, "definitions", ir->definitions, print_json_BSVec);
  JSON_PRINT_MAYBE(f, "kill_expressions", ir->kill_expressions, print_json_BSVec);

  JSON_PRINT(f, "block_count", ir->block_count, print_unsigned);
  JSON_PRINT(f, "inst_count", ir->inst_count, print_unsigned);
  JSON_PRINT(f, "reg_count", ir->reg_count, print_unsigned);
  JSON_PRINT_LAST(f, "expr_count", ir->expr_count, print_unsigned);

  JSON_PRINT_END(f);
}
