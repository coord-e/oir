#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "data_flow/data_flow.h"
#include "ir/ir.h"
#include "optimization/optimization.h"
#include "util/dummy.h"
#include "util/error.h"
#include "util/io.h"
#include "util/string.h"

typedef enum {
  INPUT_TEXT,
} InputKind;

typedef enum {
  OUTPUT_TEXT,
  OUTPUT_GRAPH,
} OutputKind;

typedef enum {
  PROC_ANALYZE_LIVENESS,
  PROC_ANALYZE_REACHING_DEFINITION,
  PROC_ANALYZE_AVAILABLE_EXPRESSION,
  PROC_OPTIMIZE_DEAD_CODE_ELIMINATION,
  PROC_OPTIMIZE_PROPAGATION,
  PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION,
} Procedure;

DECLARE_LIST(Procedure, ProcList)
DEFINE_LIST(dummy_release_int, Procedure, ProcList)

static ProcList* parse_procedures(const char* input) {
  ProcList* list = new_ProcList();

  const char* start  = input;
  const char* cursor = start;

  while (*start) {
    while (*cursor != ',' && *cursor != '\0') {
      cursor++;
    }

#define NOTATION(notation, proc)                                                                   \
  if (cursor - start == sizeof(notation) - 1 && memcmp(start, notation, cursor - start) == 0) {    \
    push_back_ProcList(list, proc);                                                                \
  }
    NOTATION("live", PROC_ANALYZE_LIVENESS)
    else NOTATION("liveness", PROC_ANALYZE_LIVENESS)
    else NOTATION("reach", PROC_ANALYZE_REACHING_DEFINITION)
    else NOTATION("reaching_definition", PROC_ANALYZE_REACHING_DEFINITION)
    else NOTATION("available", PROC_ANALYZE_AVAILABLE_EXPRESSION)
    else NOTATION("available_expression", PROC_ANALYZE_AVAILABLE_EXPRESSION)
    else NOTATION("dce", PROC_OPTIMIZE_DEAD_CODE_ELIMINATION)
    else NOTATION("dead_code_elimination", PROC_OPTIMIZE_DEAD_CODE_ELIMINATION)
    else NOTATION("prop", PROC_OPTIMIZE_PROPAGATION)
    else NOTATION("propagation", PROC_OPTIMIZE_PROPAGATION)
    else NOTATION("cse", PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION)
    else NOTATION("common_subexpression_elimination", PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION)
    else {
      error("unknown procedure: %s", strndup(start, cursor - start));
    }
#undef NOTATION

    if (*cursor == ',') {
      cursor++;
    }
    start = cursor;
  }

  return list;
}

typedef struct {
  char* input_file;
  InputKind input_format;

  char* output_file;
  OutputKind output_format;

  ProcList* procedures;

  bool help;
} Options;

static Options* new_Options() {
  Options* opts       = calloc(1, sizeof(Options));
  opts->input_file    = strdup("-");
  opts->input_format  = INPUT_TEXT;
  opts->output_file   = strdup("-");
  opts->output_format = OUTPUT_TEXT;
  opts->procedures    = parse_procedures("reach,prop,available,cse,liveness,dce");
  return opts;
}

static void release_Options(Options* opts) {
  if (opts == NULL) {
    return;
  }
  free(opts->input_file);
  free(opts->output_file);
  release_ProcList(opts->procedures);
  free(opts);
}

static void parse_args(int argc, char** argv, Options* opts) {
  int c;
  opterr = 1;
  while ((c = getopt(argc, argv, "i:o:f:g:p:h")) != -1) {
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
        if (strcmp(optarg, "text") == 0) {
          opts->input_format = INPUT_TEXT;
        } else {
          error("unknown input format: %s", optarg);
        }
        break;
      case 'g':
        if (strcmp(optarg, "text") == 0) {
          opts->output_format = OUTPUT_TEXT;
        } else if (strcmp(optarg, "graph") == 0) {
          opts->output_format = OUTPUT_GRAPH;
        } else {
          error("unknown output format: %s", optarg);
        }
        break;
      case 'p':
        release_ProcList(opts->procedures);
        opts->procedures = parse_procedures(optarg);
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
    case OUTPUT_GRAPH:
      print_graph_OIR(f, ir);
      break;
    default:
      OIR_UNREACHABLE;
  }
  close_file(f, opts->output_file);
}

static void run_procedures(Options* opts, OIR* ir) {
  FOR_EACH (Procedure, p, ProcList, opts->procedures) {
    switch (p) {
      case PROC_ANALYZE_LIVENESS:
        data_flow_liveness(ir);
        break;
      case PROC_ANALYZE_REACHING_DEFINITION:
        data_flow_reaching_definition(ir);
        break;
      case PROC_ANALYZE_AVAILABLE_EXPRESSION:
        data_flow_available_expression(ir);
        break;
      case PROC_OPTIMIZE_DEAD_CODE_ELIMINATION:
        optimization_dead_code_elimination(ir);
        break;
      case PROC_OPTIMIZE_PROPAGATION:
        optimization_propagation(ir);
        break;
      case PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION:
        optimization_common_subexpression_elimination(ir);
        break;
      default:
        OIR_UNREACHABLE;
    }
  }
}

int main(int argc, char** argv) {
  Options* opts = new_Options();
  parse_args(argc, argv, opts);

  OIR* ir = input(opts);
  run_procedures(opts, ir);
  output(opts, ir);

  release_OIR(ir);
  release_Options(opts);

  return 0;
}
