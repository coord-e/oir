#ifndef OIR_UTIL_STRING_H
#define OIR_UTIL_STRING_H

#include <stddef.h>

unsigned hash_string(const char*);
char* strdup(const char* s);
char* strndup(const char* s, size_t n);

#endif
