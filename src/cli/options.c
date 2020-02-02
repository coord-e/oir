#include "cli/options.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli/input_format.h"
#include "cli/output_format.h"
#include "cli/pass_list.h"
#include "util/convention.h"
#include "util/string.h"

Options* default_Options() {
  Options* opts         = calloc(1, sizeof(Options));
  opts->input_file      = strdup("-");
  opts->input_format    = INPUT_TEXT;
  opts->output_file     = strdup("-");
  opts->output_format   = OUTPUT_TEXT;
  opts->passes          = parse_PassList("reach,prop,available,cse,liveness,dce");
  opts->number_of_loops = 1;
  return opts;
}

void release_Options(Options* opts) {
  if (opts == NULL) {
    return;
  }
  free(opts->input_file);
  free(opts->output_file);
  release_PassList(opts->passes);
  free(opts);
}

static void print_Options_with_indent(FILE* f, Options* opts, int indent) {
#define PRINT_OPTION(opt, member, print_function)                                                  \
  fprintf(f, "%*s%s [%s (", indent, "", opt, #member);                                             \
  print_function(f, opts->member);                                                                 \
  fprintf(f, ")]\n");

  PRINT_OPTION("-i", input_file, print_string)
  PRINT_OPTION("-f", input_format, print_InputFormat)
  PRINT_OPTION("-o", output_file, print_string)
  PRINT_OPTION("-g", output_format, print_OutputFormat)
  PRINT_OPTION("-p", passes, print_PassList)
  PRINT_OPTION("-l", number_of_loops, print_unsigned)
  PRINT_OPTION("-h", help, print_bool)

#undef PRINT_OPTION
}

void print_Options(FILE* f, Options* opts) {
  print_Options_with_indent(f, opts, 0);
}

void print_usage(FILE* f, const char* program) {
  fprintf(f, "usage: %s [options]\n\n", program);
  Options* default_opts = default_Options();
  print_Options_with_indent(f, default_opts, 4);
  release_Options(default_opts);
}

Options* parse_args(int argc, char** argv) {
  Options* opts = default_Options();

  int c;
  opterr = 1;
  while ((c = getopt(argc, argv, "i:o:f:g:p:l:h")) != -1) {
    switch (c) {
      case 'i':
        free(opts->input_file);
        opts->input_file = strdup(optarg);
        break;
      case 'o':
        free(opts->output_file);
        opts->output_file = strdup(optarg);
        break;
      case 'f':
        opts->input_format = parse_InputFormat(optarg);
        break;
      case 'g':
        opts->output_format = parse_OutputFormat(optarg);
        break;
      case 'p':
        release_PassList(opts->passes);
        opts->passes = parse_PassList(optarg);
        break;
      case 'l':
        opts->number_of_loops = strtol(optarg, NULL, 10);
        break;
      case 'h':
        opts->help = true;
        break;
      case '?':
        print_usage(stderr, argv[0]);
        exit(1);
        break;
      default:
        OIR_UNREACHABLE;
    }
  }

  return opts;
}
