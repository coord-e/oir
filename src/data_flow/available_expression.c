#include "data_flow/available_expression.h"

static void number_expressions(OIR*);
static void compute_local_available_sets(OIR*);
static void compute_global_available_sets(OIR*);
static void compute_inst_available_sets(OIR*);

void data_flow_available_expression(OIR* ir) {
  // compute `ir->kill_expressions` and `inst->expression_id`
  number_expressions(ir);

  // compute `available_gen` and `available_kill` in `BasicBlock`
  compute_local_available_sets(ir);

  // compute `available_out` and `available_in` in `BasicBlock`
  compute_global_available_sets(ir);

  // compute `available_out` and `available_in` in `IRInst`
  compute_inst_available_sets(ir);
}

static void compute_expression_id(InstRefVec* exprs, Inst* inst) {
  FOR_EACH_VEC (Inst*, cmpi, InstRefVec, exprs) {
    if (compare_expressions(cmpi, inst)) {
      inst->expression_id = i_cmpi;
      return;
    }
  }
  inst->expression_id = length_InstRefVec(exprs);
  push_InstRefVec(exprs, inst);
}

void number_expressions(OIR* ir) {
  InstRefVec* exprs = new_InstRefVec(ir->inst_count);

  BSVec* kill_exprs = new_BSVec(ir->reg_count);
  for (unsigned i = 0; i < ir->reg_count; i++) {
    push_BSVec(kill_exprs, new_BitSet(ir->inst_count));
  }

  FOR_EACH (Inst*, inst, InstList, ir->instructions) {
    if (!is_expression(inst)) {
      continue;
    }
    compute_expression_id(exprs, inst);
    FOR_EACH_VEC (Reg*, r, RegVec, inst->rs) {
      set_BitSet(get_BSVec(kill_exprs, r->virtual), inst->expression_id, true);
    }
  }

  ir->expr_count = length_InstRefVec(exprs);
  release_InstRefVec(exprs);

  FOR_EACH_VEC (BitSet*, kill, BSVec, kill_exprs) { resize_BitSet(kill, ir->expr_count); }
  release_BSVec(ir->kill_expressions);
  ir->kill_expressions = kill_exprs;
}

void compute_local_available_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    block->available_gen  = new_BitSet(ir->expr_count);
    block->available_kill = new_BitSet(ir->expr_count);

    FOR_EACH (Inst*, inst, InstRange, block->instructions) {
      if (inst->rd != NULL) {
        BitSet* kill = get_BSVec(ir->kill_expressions, inst->rd->virtual);
        or_BitSet(block->available_kill, kill);
      }
      if (is_expression(inst)) {
        set_BitSet(block->available_kill, inst->expression_id, false);
        set_BitSet(block->available_gen, inst->expression_id, true);
      }
    }
  }
}

void compute_global_available_sets(OIR* ir) {
  // temporary vector to detect changes in `available_out`
  BSVec* lasts = new_BSVec(ir->block_count);
  resize_BSVec(lasts, ir->block_count);
  fill_BSVec(lasts, NULL);

  // initialization
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    release_BitSet(block->available_out);
    block->available_out = new_BitSet(ir->expr_count);
    fill_BitSet(block->available_out);
  }

  bool changed;
  do {
    changed = false;

    FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
      release_BitSet(block->available_in);
      block->available_in = new_BitSet(ir->expr_count);
      if (is_empty_BBRefList(block->preds)) {
        clear_BitSet(block->available_in);
      } else {
        fill_BitSet(block->available_in);
      }

      FOR_EACH (BasicBlock*, pre, BBRefList, block->preds) {
        and_BitSet(block->available_in, pre->available_out);
      }

      copy_to_BitSet(block->available_out, block->available_in);
      diff_BitSet(block->available_out, block->available_kill);
      or_BitSet(block->available_out, block->available_gen);

      BitSet* last = get_BSVec(lasts, block->id);
      changed      = changed || !(last && equal_to_BitSet(block->available_out, last));
      release_BitSet(last);
      set_BSVec(lasts, block->id, copy_BitSet(block->available_out));
    }
  } while (changed);

  release_BSVec(lasts);
}

void compute_inst_available_sets(OIR* ir) {
  FOR_EACH (BasicBlock*, block, BBList, ir->blocks) {
    BitSet* available = copy_BitSet(block->available_in);
    FOR_EACH (Inst*, inst, InstRange, block->instructions) {
      release_BitSet(inst->available_in);
      inst->available_in = copy_BitSet(available);

      if (inst->rd != NULL) {
        BitSet* kill = get_BSVec(ir->kill_expressions, inst->rd->virtual);
        diff_BitSet(available, kill);
      }
      if (is_expression(inst)) {
        set_BitSet(available, inst->expression_id, true);
      }

      release_BitSet(inst->available_out);
      inst->available_out = copy_BitSet(available);
    }
    assert(equal_to_BitSet(block->available_out, available));
  }
}
