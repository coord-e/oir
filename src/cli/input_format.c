#include "cli/input_format.h"

#include <stdio.h>
#include <string.h>

#include "util/error.h"

void print_InputFormat(FILE* f, InputFormat k) {
  switch (k) {
    case INPUT_TEXT:
      fputs("text", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

InputFormat parse_InputFormat(const char* input) {
  if (strcmp(input, "text") == 0) {
    return INPUT_TEXT;
  } else {
    error("unknown input format: %s (available formats: text)", input);
  }
}
