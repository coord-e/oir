#include "ir/inst.h"

#include "ir/basic_block.h"

DEFINE_VECTOR(release_Reg, Reg*, RegVec)

Inst* new_Inst(unsigned id, InstKind kind) {
  Inst* inst = calloc(1, sizeof(Inst));
  inst->id   = id;
  inst->kind = kind;
  inst->rs   = new_RegVec(2);
  return inst;
}

void release_Inst(Inst* inst) {
  if (inst == NULL) {
    return;
  }
  release_Reg(inst->rd);
  release_RegVec(inst->rs);
  free(inst->label_name);
  free(inst->then_name);
  free(inst->else_name);
  free(inst->jump_name);
  free(inst);
}

void print_Inst(FILE* p, Inst* inst) {
  if (inst->kind != IR_LABEL) {
    fprintf(p, "  ");
  }

  if (inst->rd != NULL) {
    print_Reg(p, inst->rd);
    fprintf(p, " = ");
  }

  switch (inst->kind) {
    case IR_MOV:
      fprintf(p, "MOV ");
      print_Reg(p, get_RegVec(inst->rs, 0));
      break;
    case IR_IMM:
      fprintf(p, "IMM %ld", inst->imm);
      break;
    case IR_ADD:
      fprintf(p, "ADD ");
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " ");
      print_Reg(p, get_RegVec(inst->rs, 1));
      break;
    case IR_EQUAL:
      fprintf(p, "EQUAL ");
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " ");
      print_Reg(p, get_RegVec(inst->rs, 1));
      break;
    case IR_LABEL:
      fprintf(p, "LABEL %s  # %d", inst->label_name, inst->label->id);
      break;
    case IR_BRANCH:
      fprintf(p, "BRANCH ");
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " %s %s  # %d, %d", inst->then_name, inst->else_name, inst->then_->id,
              inst->else_->id);
      break;
    case IR_JUMP:
      fprintf(p, "JUMP %s  # %d", inst->jump_name, inst->jump->id);
      break;
    case IR_RETURN:
      fprintf(p, "RETURN ");
      print_Reg(p, get_RegVec(inst->rs, 0));
      break;
    default:
      OIR_UNREACHABLE;
  }
}
