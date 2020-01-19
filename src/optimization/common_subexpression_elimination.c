#include "optimization/common_subexpression_elimination.h"

static Inst* new_mov(OIR* ir, Reg* lhs, Reg* rhs) {
  Inst* inst = new_Inst(ir->inst_count++, IR_MOV);
  push_RegVec(inst->rs, rhs);
  inst->rd = lhs;
  return inst;
}

static Reg* get_def_reg(OIR* ir, RegVec* def_regs, unsigned expr_id) {
  Reg* r = get_RegVec(def_regs, expr_id);
  if (r == NULL) {
    Reg* new_reg = new_virtual_Reg(ir->reg_count++);
    set_RegVec(def_regs, expr_id, new_reg);
    return new_reg;
  } else {
    return r;
  }
}

void optimization_common_subexpression_elimination(OIR* ir) {
  RegVec* def_regs = new_RegVec(ir->expr_count);
  resize_RegVec(def_regs, ir->expr_count);
  fill_RegVec(def_regs, NULL);

  FOR_EACH (Inst*, inst, InstList, ir->instructions) {
    if (!is_expression(inst)) {
      continue;
    }

    Reg* def_reg = copy_Reg(get_def_reg(ir, def_regs, inst->expression_id));
    if (get_BitSet(inst->available_in, inst->expression_id)) {
      inst->kind = IR_MOV;
      resize_RegVec(inst->rs, 0);
      push_RegVec(inst->rs, def_reg);
    } else {
      Inst* mov = new_mov(ir, def_reg, copy_Reg(inst->rd));
      it_inst   = insert_InstListIterator(ir->instructions, next_InstListIterator(it_inst), mov);
    }
  }

  release_RegVec(def_regs);
}
