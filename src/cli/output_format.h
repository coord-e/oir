#ifndef OIR_CLI_OUTPUT_FORMAT_H
#define OIR_CLI_OUTPUT_FORMAT_H

#include <stdio.h>

typedef enum {
  OUTPUT_TEXT,
  OUTPUT_GRAPH,
} OutputFormat;

void print_OutputFormat(FILE*, OutputFormat);
OutputFormat parse_OutputFormat(const char*);

#endif
