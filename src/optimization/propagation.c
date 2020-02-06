#include "optimization/propagation.h"

static bool get_one_definition(OIR* ir, Inst* inst, Reg* r, Inst** out) {
  BitSet* defs = copy_BitSet(get_BSVec(ir->definitions, r->virtual));
  and_BitSet(defs, inst->reach_in);
  if (count_BitSet(defs) != 1) {
    return false;
  }
  unsigned id = mssb_BitSet(defs);
  *out        = get_InstList(ir->instructions, id);
  return true;
}

void optimization_propagation(OIR* ir) {
  FOR_EACH (Inst*, inst, InstList, ir->instructions) {
    InstRefVec* defs = new_InstRefVec(length_RegVec(inst->rs));
    resize_InstRefVec(defs, length_RegVec(inst->rs));
    fill_InstRefVec(defs, NULL);

    FOR_EACH_VEC (Reg*, r, RegVec, inst->rs) {
      get_one_definition(ir, inst, r, ptr_InstRefVec(defs, i_r));
    }

    switch (inst->kind) {
      case IR_MOV: {
        Inst* def = get_InstRefVec(defs, 0);
        if (def != NULL && def->kind == IR_IMM) {
          inst->kind = IR_IMM;
          inst->imm  = def->imm;
          resize_RegVec(inst->rs, 0);
        }
        break;
      }
      case IR_ADD: {
        Inst* lhs_def = get_InstRefVec(defs, 0);
        Inst* rhs_def = get_InstRefVec(defs, 1);
        if (lhs_def != NULL && rhs_def != NULL && lhs_def->kind == IR_IMM &&
            rhs_def->kind == IR_IMM) {
          inst->kind = IR_IMM;
          inst->imm  = lhs_def->imm + rhs_def->imm;
          resize_RegVec(inst->rs, 0);
        }
        break;
      }
      case IR_EQUAL: {
        Inst* lhs_def = get_InstRefVec(defs, 0);
        Inst* rhs_def = get_InstRefVec(defs, 1);
        if (lhs_def != NULL && rhs_def != NULL && lhs_def->kind == IR_IMM &&
            rhs_def->kind == IR_IMM) {
          inst->kind = IR_IMM;
          inst->imm  = (long)(lhs_def->imm == rhs_def->imm);
          resize_RegVec(inst->rs, 0);
        }
        break;
      }
      case IR_BRANCH: {
        Inst* def = get_InstRefVec(defs, 0);
        if (def != NULL && def->kind == IR_IMM) {
          inst->kind = IR_JUMP;
          if (def->imm) {
            inst->jump      = inst->then_;
            inst->jump_name = inst->then_name;
          } else {
            inst->jump      = inst->else_;
            inst->jump_name = inst->else_name;
          }
          inst->then_ = inst->else_ = NULL;
          inst->then_name = inst->else_name = NULL;
          resize_RegVec(inst->rs, 0);
        }
        break;
      }
      default:
        break;
    }

    FOR_EACH_VEC (Reg*, r, RegVec, inst->rs) {
      Inst* def = get_InstRefVec(defs, i_r);
      if (def != NULL && def->kind == IR_MOV) {
        Reg* def_r = copy_Reg(get_RegVec(def->rs, 0));
        release_Reg(r);
        set_RegVec(inst->rs, i_r, def_r);
      }
    }

    release_InstRefVec(defs);
  }
}
