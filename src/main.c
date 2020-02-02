#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "data_flow/data_flow.h"
#include "ir/ir.h"
#include "optimization/optimization.h"
#include "util/error.h"
#include "util/io.h"
#include "util/string.h"

typedef enum {
  INPUT_TEXT,
} InputKind;

typedef enum {
  OUTPUT_TEXT,
  OUTPUT_GRAPHVIZ,
} OutputKind;

typedef struct {
  char* input_file;
  InputKind input_format;

  char* output_file;
  OutputKind output_format;

  bool help;
} Options;

static Options* new_Options() {
  Options* opts       = calloc(1, sizeof(Options));
  opts->input_file    = strdup("-");
  opts->input_format  = INPUT_TEXT;
  opts->output_file   = strdup("-");
  opts->output_format = OUTPUT_TEXT;
  return opts;
}

static void release_Options(Options* opts) {
  if (opts == NULL) {
    return;
  }
  free(opts->input_file);
  free(opts->output_file);
  free(opts);
}

static void parse_args(int argc, char** argv, Options* opts) {
  int c;
  opterr = 1;
  while ((c = getopt(argc, argv, "i:o:h")) != -1) {
    switch (c) {
      case 'i':
        free(opts->input_file);
        opts->input_file = strdup(optarg);
        break;
      case 'o':
        free(opts->output_file);
        opts->output_file = strdup(optarg);
        break;
      case 'h':
        opts->help = true;
        break;
      case '?':
        error("could not parse command line arguments");
        break;
      default:
        OIR_UNREACHABLE;
    }
  }
}

static OIR* input(Options* opts) {
  char* input = read_file(opts->input_file);
  OIR* ir;
  switch (opts->input_format) {
    case INPUT_TEXT:
      ir = parse_OIR(input);
      break;
    default:
      OIR_UNREACHABLE;
  }
  free(input);
  return ir;
}

static void output(Options* opts, OIR* ir) {
  FILE* f = open_file(opts->output_file, "wb");
  switch (opts->output_format) {
    case OUTPUT_TEXT:
      print_OIR(f, ir);
      break;
    case OUTPUT_GRAPHVIZ:
      print_graph_OIR(f, ir);
      break;
    default:
      OIR_UNREACHABLE;
  }
  close_file(f, opts->output_file);
}

int main(int argc, char** argv) {
  Options* opts = new_Options();
  parse_args(argc, argv, opts);

  OIR* ir = input(opts);

  data_flow_reaching_definition(ir);
  optimization_propagation(ir);
  data_flow_available_expression(ir);
  optimization_common_subexpression_elimination(ir);
  data_flow_liveness(ir);
  optimization_dead_code_elimination(ir);

  output(opts, ir);

  release_OIR(ir);
  release_Options(opts);

  return 0;
}
