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
} InputFormat;

static void print_InputFormat(FILE* f, InputFormat k) {
  switch (k) {
    case INPUT_TEXT:
      fputs("text", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

typedef enum {
  OUTPUT_TEXT,
  OUTPUT_GRAPH,
} OutputFormat;

static void print_OutputFormat(FILE* f, OutputFormat k) {
  switch (k) {
    case OUTPUT_TEXT:
      fputs("text", f);
      break;
    case OUTPUT_GRAPH:
      fputs("graph", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

typedef enum {
  PROC_ANALYZE_LIVENESS,
  PROC_ANALYZE_REACHING_DEFINITION,
  PROC_ANALYZE_AVAILABLE_EXPRESSION,
  PROC_OPTIMIZE_DEAD_CODE_ELIMINATION,
  PROC_OPTIMIZE_PROPAGATION,
  PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION,
} Procedure;

static void print_Procedure(FILE* f, Procedure p) {
  switch (p) {
    case PROC_ANALYZE_LIVENESS:
      fputs("liveness", f);
      break;
    case PROC_ANALYZE_REACHING_DEFINITION:
      fputs("reaching_definition", f);
      break;
    case PROC_ANALYZE_AVAILABLE_EXPRESSION:
      fputs("available_expression", f);
      break;
    case PROC_OPTIMIZE_DEAD_CODE_ELIMINATION:
      fputs("dead_code_elimination", f);
      break;
    case PROC_OPTIMIZE_PROPAGATION:
      fputs("propagation", f);
      break;
    case PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION:
      fputs("common_subexpression_elimination", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

DECLARE_LIST(Procedure, ProcList)
DEFINE_LIST(dummy_release_int, Procedure, ProcList)

static void print_ProcList(FILE* f, ProcList* list) {
  FOR_EACH (Procedure, p, ProcList, list) {
    print_Procedure(f, p);
    if (!is_nil_ProcListIterator(next_ProcListIterator(it_p))) {
      fprintf(f, ",");
    }
  }
}

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
  InputFormat input_format;

  char* output_file;
  OutputFormat output_format;

  ProcList* procedures;
  unsigned number_of_loops;

  bool help;
} Options;

static Options* new_Options() {
  Options* opts         = calloc(1, sizeof(Options));
  opts->input_file      = strdup("-");
  opts->input_format    = INPUT_TEXT;
  opts->output_file     = strdup("-");
  opts->output_format   = OUTPUT_TEXT;
  opts->procedures      = parse_procedures("reach,prop,available,cse,liveness,dce");
  opts->number_of_loops = 1;
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

static void print_unsigned(FILE* f, unsigned i) {
  fprintf(f, "%u", i);
}

static void print_string(FILE* f, const char* s) {
  fprintf(f, "%s", s);
}

static void print_bool(FILE* f, bool b) {
  if (b) {
    fprintf(f, "true");
  } else {
    fprintf(f, "false");
  }
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
  PRINT_OPTION("-p", procedures, print_ProcList)
  PRINT_OPTION("-l", number_of_loops, print_unsigned)
  PRINT_OPTION("-h", help, print_bool)

#undef PRINT_OPTION
}

static void print_usage(FILE* f, const char* program) {
  fprintf(f, "usage: %s [options]\n\n", program);
  Options* default_opts = new_Options();
  print_Options_with_indent(f, default_opts, 4);
  release_Options(default_opts);
}

static void parse_args(int argc, char** argv, Options* opts) {
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
  for (unsigned i = 0; i < opts->number_of_loops; i++) {
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
}

int main(int argc, char** argv) {
  Options* opts = new_Options();
  parse_args(argc, argv, opts);

  if (opts->help) {
    print_usage(stdout, argv[0]);
    exit(0);
  }

  OIR* ir = input(opts);
  run_procedures(opts, ir);
  output(opts, ir);

  release_OIR(ir);
  release_Options(opts);

  return 0;
}
