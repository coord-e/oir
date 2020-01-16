#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_flow.h"
#include "ir.h"
#include "parser.h"

static bool is_hyphen(const char* path) {
  return path[0] == '-' && path[1] == '\0';
}

static FILE* open_file(const char* path, const char* mode) {
  if (mode[0] == 'w' && is_hyphen(path)) {
    return stdout;
  }
  FILE* f = fopen(path, mode);
  if (f == NULL) {
    error("could not open \"%s\": %s", path, strerror(errno));
  }
  return f;
}

static void close_file(FILE* f) {
  if (f == stdout) {
    return;
  }
  fclose(f);
}

static char* read_file(const char* path) {
  FILE* f = open_file(path, "rb");
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char* buf = malloc(size + 1);
  fread(buf, 1, size, f);
  fclose(f);

  buf[size] = 0;

  return buf;
}

int main(int argc, char** argv) {
  char* input = read_file(argv[1]);
  OIR* ir     = parse(input);
  data_flow_liveness(ir);
  data_flow_reaching_definition(ir);
  print_OIR(stdout, ir);
  release_OIR(ir);
  free(input);
  return 0;
}
