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

#define FOR_EACH(T, item, Name, list)                                                              \
  for (bool keep = 1; keep; keep = !keep)                                                          \
    for (Name##Iterator* it_##item = front_##Name(list);                                           \
         keep && !is_nil_##Name##Iterator(it_##item);                                              \
         keep = !keep, it_##item = next_##Name##Iterator(it_##item))                               \
      for (T item = data_##Name##Iterator(it_##item); keep; keep = !keep)

#endif
