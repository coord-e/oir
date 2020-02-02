#include "cli/pass_list.h"

#include <stdio.h>

#include "cli/pass.h"
#include "container/list.h"
#include "util/dummy.h"
#include "util/string.h"

DEFINE_LIST(dummy_release_int, Pass, PassList)

void print_PassList(FILE* f, PassList* list) {
  FOR_EACH (Pass, p, PassList, list) {
    print_Pass(f, p);
    if (!is_nil_PassListIterator(next_PassListIterator(it_p))) {
      fprintf(f, ",");
    }
  }
}

PassList* parse_PassList(const char* input) {
  PassList* list = new_PassList();

  const char* start  = input;
  const char* cursor = start;

  while (*start) {
    while (*cursor != ',' && *cursor != '\0') {
      cursor++;
    }

    char* name = strndup(start, cursor - start);
    Pass p     = parse_Pass(name);
    free(name);

    push_back_PassList(list, p);

    if (*cursor == ',') {
      cursor++;
    }
    start = cursor;
  }

  return list;
}
