#ifndef OIR_CLI_PROC_LIST_H
#define OIR_CLI_PROC_LIST_H

#include <stdio.h>

#include "cli/procedure.h"
#include "container/list.h"

DECLARE_LIST(Procedure, ProcList)

void print_ProcList(FILE*, ProcList*);
ProcList* parse_ProcList(const char*);

#endif
