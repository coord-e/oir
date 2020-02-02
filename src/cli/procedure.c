#include "cli/procedure.h"

#include <stdio.h>
#include <string.h>

#include "util/error.h"

void print_Procedure(FILE* f, Procedure p) {
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

Procedure parse_Procedure(const char* input) {
#define NOTATION(name1, name2, proc)                                                               \
  if (strcmp(input, name1) == 0 || strcmp(input, name2) == 0) {                                    \
    return proc;                                                                                   \
  }

  NOTATION("live", "liveness", PROC_ANALYZE_LIVENESS)
  else NOTATION("reach", "reaching_definition", PROC_ANALYZE_REACHING_DEFINITION)
  else NOTATION("available", "available_expression", PROC_ANALYZE_AVAILABLE_EXPRESSION)
  else NOTATION("dce", "dead_code_elimination", PROC_OPTIMIZE_DEAD_CODE_ELIMINATION)
  else NOTATION("prop", "propagation", PROC_OPTIMIZE_PROPAGATION)
  else NOTATION("cse", "common_subexpression_elimination", PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION)
  else {
    error("unknown procedure: %s", input);
  }

#undef NOTATION
}
