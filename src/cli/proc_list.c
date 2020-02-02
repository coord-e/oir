#include "cli/proc_list.h"

#include <stdio.h>

#include "cli/procedure.h"
#include "container/list.h"
#include "util/dummy.h"
#include "util/string.h"

DEFINE_LIST(dummy_release_int, Procedure, ProcList)

void print_ProcList(FILE* f, ProcList* list) {
  FOR_EACH (Procedure, p, ProcList, list) {
    print_Procedure(f, p);
    if (!is_nil_ProcListIterator(next_ProcListIterator(it_p))) {
      fprintf(f, ",");
    }
  }
}

ProcList* parse_ProcList(const char* input) {
  ProcList* list = new_ProcList();

  const char* start  = input;
  const char* cursor = start;

  while (*start) {
    while (*cursor != ',' && *cursor != '\0') {
      cursor++;
    }

    char* name  = strndup(start, cursor - start);
    Procedure p = parse_Procedure(name);
    free(name);

    push_back_ProcList(list, p);

    if (*cursor == ',') {
      cursor++;
    }
    start = cursor;
  }

  return list;
}
