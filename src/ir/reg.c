#include "ir/reg.h"

#include <stdlib.h>


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
