#include "ir.h"
#include "util.h"


static Reg* new_Reg(RegKind kind) {
  Reg* r = calloc(1, sizeof(Reg));
  r->kind = kind;
  return r;
}

Reg* new_virtual_Reg(unsigned virtual) {
  Reg* r = new_Reg(REG_VIRT);
  r->virtual = virtual;
  return r;
}

Reg* new_real_Reg(unsigned real) {
  Reg* r = new_Reg(REG_REAL);
  r->real = real;
  return r;
}

Reg* copy_Reg(Reg* r) {
  Reg* copy = new_Reg(r->kind);
  *copy = *r;
  return copy;
}

void release_Reg(Reg* r) {
  if (r == NULL) {
    return;
  }
  free(r);
}

DEFINE_VECTOR(release_Reg, Reg*, RegVec)

Inst* new_Inst(unsigned id, InstKind kind) {
  Inst* inst = calloc(1, sizeof(Inst));
  inst->id = id;
  inst->kind = kind;
  inst->rs = new_RegVec(2);
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

DEFINE_LIST(release_void, BasicBlock*, BBRefList)
DEFINE_LIST(release_Inst, Inst*, InstList)
DEFINE_RANGE(Inst*, InstList, InstRange)

BasicBlock* new_BasicBlock(unsigned id) {
  BasicBlock* block = calloc(1, sizeof(BasicBlock));
  block->id = id;
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

DEFINE_LIST(release_BasicBlock, BasicBlock*, BBList)

OIR* new_OIR() {
  OIR* ir = calloc(1, sizeof(OIR));
  ir->blocks = new_BBList();
  ir->instructions = new_InstList();
  return ir;
}

void release_OIR(OIR* ir) {
  if (ir == NULL) {
    return;
  }
  release_BBList(ir->blocks);
  release_InstList(ir->instructions);
  free(ir);
}
