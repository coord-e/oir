#include "util/convention.h"

#include <stdio.h>

void print_unsigned(FILE* f, unsigned i) {
  fprintf(f, "%u", i);
}

void print_string(FILE* f, const char* s) {
  fprintf(f, "%s", s);
}

void print_bool(FILE* f, bool b) {
  if (b) {
    fprintf(f, "true");
  } else {
    fprintf(f, "false");
  }
}
