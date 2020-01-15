#ifndef OIR_IR_REG_H
#define OIR_IR_REG_H

#include <stdio.h>

typedef enum {
  REG_VIRT,
  REG_REAL,
} RegKind;

typedef struct {
  RegKind kind;
  unsigned virtual;
  unsigned real;
} Reg;

Reg* new_virtual_Reg(unsigned virtual);
Reg* new_real_Reg(unsigned real);
Reg* copy_Reg(Reg*);
void release_Reg(Reg*);
void print_Reg(FILE*, Reg*);

#endif
