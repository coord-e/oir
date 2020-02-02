#ifndef OIR_CLI_OPTIONS_H
#define OIR_CLI_OPTIONS_H

#include "cli/input_format.h"
#include "cli/output_format.h"
#include "cli/pass_list.h"

typedef struct {
  char* input_file;
  InputFormat input_format;

  char* output_file;
  OutputFormat output_format;

  PassList* passes;
  unsigned number_of_loops;

  bool help;
} Options;

Options* default_Options();
void release_Options(Options*);
void print_Options(FILE*, Options*);

void print_usage(FILE*, const char* program);
Options* parse_args(int argc, char** argv);

#endif
