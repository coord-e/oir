#ifndef OIR_UTIL_H
#define OIR_UTIL_H

#include <stdnoreturn.h>


void release_void(void*);
noreturn void error(char* fmt, ...);

#define OIR_UNREACHABLE error("unreachable (%s:%d)", __FILE__, __LINE__)

#endif
