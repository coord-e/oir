#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_flow/data_flow.h"
#include "ir/ir.h"
#include "optimization/optimization.h"

static bool is_hyphen(const char* path) {
  return path[0] == '-' && path[1] == '\0';
}

static FILE* open_file(const char* path, const char* mode) {
  if (is_hyphen(path)) {
    switch (mode[0]) {
      case 'w':
      case 'a':
        return stdout;
      case 'r':
        return stdin;
      default:
        break;
    }
  }
  FILE* f = fopen(path, mode);
  if (f == NULL) {
    error("could not open \"%s\": %s", path, strerror(errno));
  }
  return f;
}

static void close_file(FILE* f, const char* path) {
  if (f == stdout || f == stdin || f == stderr) {
    return;
  }
  if (fclose(f) == EOF) {
    error("could not close \"%s\": %s", path, strerror(errno));
  }
}

typedef struct {
  char* buffer;
  char* cursor;
  size_t chunk_size;
  size_t last_read_size;
} Buffer;

static Buffer* new_Buffer(size_t chunk_size) {
  Buffer* buf     = malloc(sizeof(Buffer));
  buf->buffer     = malloc(chunk_size);
  buf->cursor     = buf->buffer;
  buf->chunk_size = chunk_size;
  return buf;
}

static char* finalize_Buffer(Buffer* buf) {
  size_t buffer_size      = buf->cursor - buf->buffer + buf->last_read_size + 1;
  char* result            = realloc(buf->buffer, buffer_size);
  result[buffer_size - 1] = 0;

  free(buf);
  return result;
}

static void extend_buffer(Buffer* buf) {
  size_t current_size = buf->cursor - buf->buffer + buf->chunk_size;
  size_t next_size    = current_size + buf->chunk_size;
  assert(buf->cursor + buf->chunk_size == buf->buffer + current_size);

  buf->buffer = realloc(buf->buffer, next_size);
  buf->cursor = buf->buffer + current_size;
}

static bool read_chunk(Buffer* buf, FILE* f) {
  buf->last_read_size = fread(buf->cursor, 1, buf->chunk_size, f);
  extend_buffer(buf);
  return buf->last_read_size == buf->chunk_size;
}

static char* read_file(const char* path) {
  FILE* f = open_file(path, "rb");

  Buffer* buf = new_Buffer(5);
  while (read_chunk(buf, f)) {}
  char* result = finalize_Buffer(buf);

  if (ferror(f)) {
    close_file(f, path);
    error("could not read \"%s\": %s", path, strerror(errno));
  }

  assert(feof(f));
  close_file(f, path);
  return result;
}

int main(int argc, char** argv) {
  char* input = read_file(argv[1]);
  OIR* ir     = parse_OIR(input);
  data_flow_reaching_definition(ir);
  optimization_propagation(ir);
  data_flow_available_expression(ir);
  optimization_common_subexpression_elimination(ir);
  data_flow_liveness(ir);
  optimization_dead_code_elimination(ir);
  print_OIR(stdout, ir);
  release_OIR(ir);
  free(input);
  return 0;
}
