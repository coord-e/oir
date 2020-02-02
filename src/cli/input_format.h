#ifndef OIR_CLI_INPUT_FORMAT_H
#define OIR_CLI_INPUT_FORMAT_H

#include <stdio.h>

typedef enum {
  INPUT_TEXT,
} InputFormat;

void print_InputFormat(FILE*, InputFormat);
InputFormat parse_InputFormat(const char*);

#endif
