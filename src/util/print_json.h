#ifndef OIR_UTIL_PRINT_JSON_H
#define OIR_UTIL_PRINT_JSON_H

#include <stdio.h>

#define JSON_PRINT_START(f) fprintf(f, "{");
#define JSON_PRINT_END(f) fprintf(f, "}");

#define JSON_PRINT_LAST_IMPL(f, key, start, end, value, printer)                                   \
  fprintf(f, "\"%s\": " start, key);                                                               \
  printer(f, value);                                                                               \
  fputs(end, f);
#define JSON_PRINT_IMPL(f, key, start, end, value, printer)                                        \
  JSON_PRINT_LAST_IMPL(f, key, start, end, value, printer);                                        \
  fprintf(f, ",");

#define JSON_PRINT(f, key, value, printer) JSON_PRINT_IMPL(f, key, "", "", value, printer)
#define JSON_PRINT_STRING(f, key, value, printer)                                                  \
  JSON_PRINT_IMPL(f, key, "\"", "\"", value, printer)
#define JSON_PRINT_ARRAY(f, key, value, printer) JSON_PRINT_IMPL(f, key, "[", "]", value, printer)

#define JSON_PRINT_MAYBE(f, key, value, printer)                                                   \
  if (value != NULL) {                                                                             \
    JSON_PRINT(f, key, value, printer);                                                            \
  }
#define JSON_PRINT_ARRAY_MAYBE(f, key, value, printer)                                             \
  if (value != NULL) {                                                                             \
    JSON_PRINT_ARRAY(f, key, value, printer);                                                      \
  }

#define JSON_PRINT_LAST(f, key, value, printer) JSON_PRINT_LAST_IMPL(f, key, "", "", value, printer)
#define JSON_PRINT_ARRAY_LAST(f, key, value, printer)                                              \
  JSON_PRINT_LAST_IMPL(f, key, "[", "]", value, printer)

#endif
