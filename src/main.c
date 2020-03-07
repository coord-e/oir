#include <stdio.h>
#include <stdlib.h>

#include "cli/options.h"
#include "data_flow/data_flow.h"
#include "ir/ir.h"
#include "optimization/optimization.h"
#include "util/error.h"
#include "util/io.h"

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
    case OUTPUT_GRAPH:
      print_graph_OIR(f, ir);
      break;
    case OUTPUT_JSON:
      print_json_OIR(f, ir);
      break;
    default:
      OIR_UNREACHABLE;
  }
  close_file(f, opts->output_file);
}

static void run_passes(Options* opts, OIR* ir) {
  for (unsigned i = 0; i < opts->number_of_loops; i++) {
    FOR_EACH (Pass, p, PassList, opts->passes) {
      switch (p) {
        case PASS_ANALYZE_LIVENESS:
          data_flow_liveness(ir);
          break;
        case PASS_ANALYZE_REACHING_DEFINITION:
          data_flow_reaching_definition(ir);
          break;
        case PASS_ANALYZE_AVAILABLE_EXPRESSION:
          data_flow_available_expression(ir);
          break;
        case PASS_OPTIMIZE_DEAD_CODE_ELIMINATION:
          optimization_dead_code_elimination(ir);
          break;
        case PASS_OPTIMIZE_PROPAGATION:
          optimization_propagation(ir);
          break;
        case PASS_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION:
          optimization_common_subexpression_elimination(ir);
          break;
        default:
          OIR_UNREACHABLE;
      }
    }
  }
}

int main(int argc, char** argv) {
  Options* opts = parse_args(argc, argv);

  if (opts->help) {
    print_usage(stdout, argv[0]);
    exit(0);
  }

  OIR* ir = input(opts);
  run_passes(opts, ir);
  output(opts, ir);

  release_OIR(ir);
  release_Options(opts);

  return 0;
}
