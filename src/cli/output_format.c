#include "cli/output_format.h"

#include <stdio.h>
#include <string.h>

#include "util/error.h"

void print_OutputFormat(FILE* f, OutputFormat k) {
  switch (k) {
    case OUTPUT_TEXT:
      fputs("text", f);
      break;
    case OUTPUT_GRAPH:
      fputs("graph", f);
      break;
    case OUTPUT_JSON:
      fputs("json", f);
      break;
    default:
      OIR_UNREACHABLE;
  }
}

OutputFormat parse_OutputFormat(const char* input) {
  if (strcmp(input, "text") == 0) {
    return OUTPUT_TEXT;
  } else if (strcmp(input, "graph") == 0) {
    return OUTPUT_GRAPH;
  } else if (strcmp(input, "json") == 0) {
    return OUTPUT_JSON;
  } else {
    error("unknown output format: %s (available formats: text, graph, json)", input);
  }
}
