#ifndef OIR_UTIL_H
#define OIR_UTIL_H

#include <stddef.h>
#include <stdnoreturn.h>

void release_void(void*);
unsigned hash_string(const char*);
char* strdup(const char* s);
char* strndup(const char* s, size_t n);

noreturn void error(char* fmt, ...);

#define OIR_UNREACHABLE error("unreachable (%s:%d)", __FILE__, __LINE__)

#endif
