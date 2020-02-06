#include "util/io.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util/error.h"

static bool is_hyphen(const char* path) {
  return path[0] == '-' && path[1] == '\0';
}

FILE* open_file(const char* path, const char* mode) {
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

void close_file(FILE* f, const char* path) {
  if (f == stdout || f == stdin || f == stderr) {
    return;
  }
  if (fclose(f) == EOF) {
    error("could not close \"%s\": %s", path, strerror(errno));
  }
}

void write_file(const char* path, const char* content) {
  FILE* f = open_file(path, "wb");
  fputs(content, f);
  close_file(f, path);
}

typedef struct {
  char* buffer;
  char* cursor;
  size_t chunk_size;
  size_t actual_size;
} Buffer;

static Buffer* new_Buffer(size_t chunk_size) {
  Buffer* buf      = malloc(sizeof(Buffer));
  buf->buffer      = malloc(chunk_size);
  buf->cursor      = buf->buffer;
  buf->chunk_size  = chunk_size;
  buf->actual_size = -1;
  return buf;
}

static char* finalize_Buffer(Buffer* buf) {
  assert(buf->actual_size != -1);
  char* result                 = realloc(buf->buffer, buf->actual_size);
  result[buf->actual_size - 1] = 0;

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
  size_t read_size = fread(buf->cursor, 1, buf->chunk_size, f);
  buf->actual_size = buf->cursor - buf->buffer + read_size + 1;
  extend_buffer(buf);
  return read_size == buf->chunk_size;
}

char* read_file(const char* path) {
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
