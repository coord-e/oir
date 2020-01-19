#include "data_flow/liveness.h"

#include "container/bit_set.h"
#include "container/vector.h"

static void compute_local_live_sets(OIR*);
static void compute_global_live_sets(OIR*);
static void compute_inst_live_sets(OIR*);

void data_flow_liveness(OIR* ir) {
  // compute `live_gen` and `live_kill` in `BasicBlock`
  compute_local_live_sets(ir);

  // compute `live_out` and `live_in` in `BasicBlock`
  compute_global_live_sets(ir);

  // compute `live_out` and `live_in` in `IRInst`
  compute_inst_live_sets(ir);
}

static void compute_local_live_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    block->live_gen  = new_BitSet(ir->reg_count);
    block->live_kill = new_BitSet(ir->reg_count);

    FOR_EACH (Inst*, inst, InstRange, block->instructions) {
      FOR_EACH_VEC (Reg*, ra, RegVec, inst->rs) {
        unsigned vi = ra->virtual;
        if (!get_BitSet(block->live_kill, vi)) {
          set_BitSet(block->live_gen, vi, true);
        }
      }

      if (inst->rd != NULL) {
        set_BitSet(block->live_kill, inst->rd->virtual, true);
      }
    }
  }
}

static void compute_global_live_sets(OIR* ir) {
  // temporary vector to detect changes in `live_in`
  BSVec* lasts = new_BSVec(ir->block_count);
  resize_BSVec(lasts, ir->block_count);
  fill_BSVec(lasts, NULL);

  // initialization
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    release_BitSet(block->live_in);
    block->live_in = new_BitSet(ir->reg_count);
  }

  bool changed;
  do {
    changed = false;

    FOR_EACH_BACK (BasicBlock*, block, BBList, ir->blocks) {
      release_BitSet(block->live_out);
      block->live_out = new_BitSet(ir->reg_count);

      FOR_EACH (BasicBlock*, sux, BBRefList, block->succs) {
        or_BitSet(block->live_out, sux->live_in);
      }

      copy_to_BitSet(block->live_in, block->live_out);
      diff_BitSet(block->live_in, block->live_kill);
      or_BitSet(block->live_in, block->live_gen);

      BitSet* last = get_BSVec(lasts, block->id);
      changed      = changed || !(last && equal_to_BitSet(block->live_in, last));
      release_BitSet(last);
      set_BSVec(lasts, block->id, copy_BitSet(block->live_in));
    }
  } while (changed);

  release_BSVec(lasts);
}

static void compute_inst_live_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    BitSet* live = copy_BitSet(block->live_out);
    FOR_EACH_BACK (Inst*, inst, InstRange, block->instructions) {
      release_BitSet(inst->live_out);
      inst->live_out = copy_BitSet(live);

      if (inst->rd != NULL) {
        set_BitSet(live, inst->rd->virtual, false);
      }
      FOR_EACH_VEC (Reg*, ra, RegVec, inst->rs) { set_BitSet(live, ra->virtual, true); }

      release_BitSet(inst->live_in);
      inst->live_in = copy_BitSet(live);
    }
    assert(equal_to_BitSet(block->live_in, live));
    release_BitSet(live);
  }
}
