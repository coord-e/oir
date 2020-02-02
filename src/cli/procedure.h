#ifndef OIR_CLI_PROCEDURE_H
#define OIR_CLI_PROCEDURE_H

#include <stdio.h>

typedef enum {
  PROC_ANALYZE_LIVENESS,
  PROC_ANALYZE_REACHING_DEFINITION,
  PROC_ANALYZE_AVAILABLE_EXPRESSION,
  PROC_OPTIMIZE_DEAD_CODE_ELIMINATION,
  PROC_OPTIMIZE_PROPAGATION,
  PROC_OPTIMIZE_COMMOM_SUBEXPRESSION_ELIMINATION,
} Procedure;

void print_Procedure(FILE*, Procedure);
Procedure parse_Procedure(const char*);

#endif
