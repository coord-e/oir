#include "ir/inst.h"

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
  free(inst);
}
