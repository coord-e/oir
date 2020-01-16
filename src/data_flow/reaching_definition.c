#include "data_flow/reaching_definition.h"

static void collect_defs(OIR*);
static void compute_local_reach_sets(OIR*);
static void compute_global_reach_sets(OIR*);
static void compute_inst_reach_sets(OIR*);

void data_flow_reaching_definition(OIR* ir) {
  // compute `ir->definitions`
  collect_defs(ir);

  // compute `reach_gen` and `reach_kill` in `BasicBlock`
  compute_local_reach_sets(ir);

  // compute `reach_out` and `reach_in` in `BasicBlock`
  compute_global_reach_sets(ir);

  // compute `reach_out` and `reach_in` in `IRInst`
  compute_inst_reach_sets(ir);
}

static void collect_defs(OIR* ir) {
  BSVec* defs = new_BSVec(ir->reg_count);
  for (unsigned i = 0; i < ir->reg_count; i++) {
    push_BSVec(defs, new_BitSet(ir->inst_count));
  }

  FOR_EACH (Inst*, inst, InstList, ir->instructions) {
    if (inst->rd != NULL) {
      set_BitSet(get_BSVec(defs, inst->rd->virtual), inst->id, true);
    }
  }

  ir->definitions = defs;
}

static void compute_local_reach_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    block->reach_gen  = new_BitSet(ir->inst_count);
    block->reach_kill = new_BitSet(ir->inst_count);

    FOR_EACH_BACK (Inst*, inst, InstRange, block->instructions) {
      if (inst->rd == NULL) {
        continue;
      }

      if (!get_BitSet(block->reach_kill, inst->id)) {
        set_BitSet(block->reach_gen, inst->id, true);
      }

      BitSet* kill = copy_BitSet(get_BSVec(ir->definitions, inst->rd->virtual));
      set_BitSet(kill, inst->id, false);
      or_BitSet(block->reach_kill, kill);
      release_BitSet(kill);
    }
  }
}

static void compute_global_reach_sets(OIR* ir) {
  // temporary vector to detect changes in `reach_out`
  BSVec* lasts = new_BSVec(ir->block_count);
  for (unsigned i = 0; i < ir->block_count; i++) {
    push_BSVec(lasts, NULL);
  }

  // initialization
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    release_BitSet(block->reach_out);
    block->reach_out = new_BitSet(ir->inst_count);
  }

  bool changed;
  do {
    changed = false;

    FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
      release_BitSet(block->reach_in);
      block->reach_in = new_BitSet(ir->inst_count);

      FOR_EACH (BasicBlock*, pre, BBRefList, block->preds) {
        or_BitSet(block->reach_in, pre->reach_out);
      }

      copy_to_BitSet(block->reach_out, block->reach_in);
      diff_BitSet(block->reach_out, block->reach_kill);
      or_BitSet(block->reach_out, block->reach_gen);

      BitSet* last = get_BSVec(lasts, block->id);
      changed      = changed || !(last && equal_to_BitSet(block->reach_out, last));
      release_BitSet(last);
      set_BSVec(lasts, block->id, copy_BitSet(block->reach_out));
    }
  } while (changed);

  release_BSVec(lasts);
}

static void compute_inst_reach_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    BitSet* reach = copy_BitSet(block->reach_in);
    FOR_EACH (Inst*, inst, InstRange, block->instructions) {
      release_BitSet(inst->reach_in);
      inst->reach_in = copy_BitSet(reach);

      if (inst->rd != NULL) {
        BitSet* defs = copy_BitSet(get_BSVec(ir->definitions, inst->rd->virtual));
        set_BitSet(defs, inst->id, false);
        diff_BitSet(reach, defs);
        set_BitSet(reach, inst->id, true);
        release_BitSet(defs);
      }

      release_BitSet(inst->reach_out);
      inst->reach_out = copy_BitSet(reach);
    }
    assert(equal_to_BitSet(block->reach_out, reach));
  }
}
