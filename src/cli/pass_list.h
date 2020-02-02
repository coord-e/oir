#ifndef OIR_CLI_PASS_LIST_H
#define OIR_CLI_PASS_LIST_H

#include <stdio.h>

#include "cli/pass.h"
#include "container/list.h"

DECLARE_LIST(Pass, PassList)

void print_PassList(FILE*, PassList*);
PassList* parse_PassList(const char*);

#endif
