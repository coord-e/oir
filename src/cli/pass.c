#include "cli/pass.h"

#include <stdio.h>
#include <string.h>

#include "util/error.h"

void print_Pass(FILE* f, Pass p) {
  switch (p) {
    case PASS_ANALYZE_LIVENESS:
      fputs("live", f);
      break;
    case PASS_ANALYZE_REACHING_DEFINITION:
      fputs("reach", f);
      break;
    case PASS_ANALYZE_AVAILABLE_EXPRESSION:
      fputs("available", f);
      break;
    case PASS_OPTIMIZE_DEAD_CODE_ELIMINATION:
      fputs("dce", f);
      break;
    case PASS_OPTIMIZE_PROPAGATION:
      fputs("prop", f);
      break;
    case PASS_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION:
      fputs("cse", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

Pass parse_Pass(const char* input) {
#define NOTATION(name1, name2, pass)                                                               \
  if (strcmp(input, name1) == 0 || strcmp(input, name2) == 0) {                                    \
    return pass;                                                                                   \
  }

  NOTATION("live", "liveness", PASS_ANALYZE_LIVENESS)
  else NOTATION("reach", "reaching_definition", PASS_ANALYZE_REACHING_DEFINITION)
  else NOTATION("available", "available_expression", PASS_ANALYZE_AVAILABLE_EXPRESSION)
  else NOTATION("dce", "dead_code_elimination", PASS_OPTIMIZE_DEAD_CODE_ELIMINATION)
  else NOTATION("prop", "propagation", PASS_OPTIMIZE_PROPAGATION)
  else NOTATION("cse", "common_subexpression_elimination", PASS_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION)
  else {
    error("unknown pass: %s (available passes: live, reach, available, dce, prop, cse)", input);
  }

#undef NOTATION
}
