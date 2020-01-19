#include "ir/reg.h"

#include <stdlib.h>

#include "util/error.h"

static Reg* new_Reg(RegKind kind) {
  Reg* r  = calloc(1, sizeof(Reg));
  r->kind = kind;
  return r;
}

Reg* new_virtual_Reg(unsigned virtual) {
  Reg* r     = new_Reg(REG_VIRT);
  r->virtual = virtual;
  return r;
}

Reg* new_real_Reg(unsigned real) {
  Reg* r  = new_Reg(REG_REAL);
  r->real = real;
  return r;
}

Reg* copy_Reg(Reg* r) {
  Reg* copy = new_Reg(r->kind);
  *copy     = *r;
  return copy;
}

void release_Reg(Reg* r) {
  if (r == NULL) {
    return;
  }
  free(r);
}

void print_Reg(FILE* p, Reg* r) {
  switch (r->kind) {
    case REG_VIRT:
      fprintf(p, "v%d", r->virtual);
      break;
    case REG_REAL:
      fprintf(p, "r%d", r->real);
      break;
    default:
      OIR_UNREACHABLE;
  }
}
