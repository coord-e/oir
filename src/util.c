#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void release_void(void* x) {}

unsigned hash_string(const char* s) {
  unsigned hash = 0;
  char c;

  while ((c = *s++)) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  return hash;
}

char* strndup(const char* s, size_t n) {
  size_t size = n + 1;
  char* m     = malloc(size);
  memcpy(m, s, n);
  m[n] = '\0';
  return m;
}

char* strdup(const char* s) {
  size_t size = strlen(s) + 1;
  char* m     = malloc(size);
  strcpy(m, s);
  return m;
}

void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "ccc error: ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
