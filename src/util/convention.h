#ifndef OIR_UTIL_CONVENTION_H
#define OIR_UTIL_CONVENTION_H

#include <stdbool.h>
#include <stdio.h>

void print_unsigned(FILE*, unsigned i);
void print_long(FILE*, long i);
void print_string(FILE*, const char* s);
void print_bool(FILE*, bool b);

#endif
