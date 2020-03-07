#include "ir/inst.h"

#include "ir/basic_block.h"
#include "util/convention.h"
#include "util/print_json.h"

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
  release_BitSet(inst->live_in);
  release_BitSet(inst->live_out);
  release_BitSet(inst->reach_in);
  release_BitSet(inst->reach_out);
  release_BitSet(inst->available_in);
  release_BitSet(inst->available_out);
  free(inst->label_name);
  free(inst->then_name);
  free(inst->else_name);
  free(inst->jump_name);
  free(inst);
}

static void print_InstKind(FILE* f, InstKind k) {
  switch (k) {
    case IR_MOV:
      fputs("MOV", f);
      break;
    case IR_IMM:
      fputs("IMM", f);
      break;
    case IR_ADD:
      fputs("ADD", f);
      break;
    case IR_EQUAL:
      fputs("EQUAL", f);
      break;
    case IR_LABEL:
      fputs("LABEL", f);
      break;
    case IR_BRANCH:
      fputs("BRANCH", f);
      break;
    case IR_JUMP:
      fputs("JUMP", f);
      break;
    case IR_RETURN:
      fputs("RETURN", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

void print_Inst(FILE* p, Inst* inst) {
  if (inst->kind != IR_LABEL) {
    fprintf(p, "  ");
  }

  if (inst->rd != NULL) {
    print_Reg(p, inst->rd);
    fprintf(p, " = ");
  }

  print_InstKind(p, inst->kind);
  fprintf(p, " ");

  switch (inst->kind) {
    case IR_MOV:
      print_Reg(p, get_RegVec(inst->rs, 0));
      break;
    case IR_IMM:
      fprintf(p, "%ld", inst->imm);
      break;
    case IR_ADD:
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " ");
      print_Reg(p, get_RegVec(inst->rs, 1));
      break;
    case IR_EQUAL:
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " ");
      print_Reg(p, get_RegVec(inst->rs, 1));
      break;
    case IR_LABEL:
      fprintf(p, "%s  # %d", inst->label_name, inst->label->id);
      break;
    case IR_BRANCH:
      print_Reg(p, get_RegVec(inst->rs, 0));
      fprintf(p, " %s %s  # %d, %d", inst->then_name, inst->else_name, inst->then_->id,
              inst->else_->id);
      break;
    case IR_JUMP:
      fprintf(p, "%s  # %d", inst->jump_name, inst->jump->id);
      break;
    case IR_RETURN:
      print_Reg(p, get_RegVec(inst->rs, 0));
      break;
    default:
      OIR_UNREACHABLE;
  }
}

static void print_json_RegVec(FILE* f, RegVec* rs) {
  FOR_EACH_VEC (Reg*, r, RegVec, rs) {
    if (i_r != 0) {
      fputs(",", f);
    }
    fputs("\"", f);
    print_Reg(f, r);
    fputs("\"", f);
  }
}

void print_json_Inst(FILE* f, Inst* inst) {
  JSON_PRINT_START(f);

  JSON_PRINT_STRING(f, "kind", inst->kind, print_InstKind);
  JSON_PRINT(f, "id", inst->id, print_unsigned);

  switch (inst->kind) {
    case IR_IMM:
      JSON_PRINT(f, "imm", inst->imm, print_long);
      break;
    case IR_LABEL:
      JSON_PRINT_STRING(f, "label_name", inst->label_name, print_string);
      JSON_PRINT(f, "label_id", inst->label->id, print_unsigned);
      break;
    case IR_BRANCH:
      JSON_PRINT_STRING(f, "then_name", inst->then_name, print_string);
      JSON_PRINT(f, "then_id", inst->then_->id, print_unsigned);
      JSON_PRINT_STRING(f, "else_name", inst->else_name, print_string);
      JSON_PRINT(f, "else_id", inst->else_->id, print_unsigned);
      break;
    case IR_JUMP:
      JSON_PRINT_STRING(f, "jump_name", inst->jump_name, print_string);
      JSON_PRINT(f, "jump_id", inst->jump->id, print_unsigned);
      break;
    default:
      break;
  }

  if (inst->rd != NULL) {
    JSON_PRINT_STRING(f, "rd", inst->rd, print_Reg);
  }

  JSON_PRINT_ARRAY(f, "rs", inst->rs, print_json_RegVec);

  JSON_PRINT_ARRAY_MAYBE(f, "live_in", inst->live_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "live_out", inst->live_out, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_in", inst->reach_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "reach_out", inst->reach_out, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_in", inst->available_in, print_BitSet);
  JSON_PRINT_ARRAY_MAYBE(f, "available_out", inst->available_out, print_BitSet);

  JSON_PRINT_LAST(f, "expression_id", inst->expression_id, print_unsigned);

  JSON_PRINT_END(f);
}

bool is_expression(Inst* inst) {
  return inst->kind == IR_ADD || inst->kind == IR_EQUAL;
}

bool compare_expressions(Inst* a, Inst* b) {
  assert(is_expression(a) && is_expression(b));

  if (a->kind != b->kind) {
    return false;
  }

  FOR_EACH_VEC (Reg*, ra, RegVec, a->rs) {
    Reg* rb = get_RegVec(b->rs, i_ra);
    if (ra->virtual != rb->virtual) {
      return false;
    }
  }

  return true;
}
