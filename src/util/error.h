#ifndef OIR_UTIL_ERROR_H
#define OIR_UTIL_ERROR_H

#include <stdnoreturn.h>

noreturn void error(char* fmt, ...);

#define OIR_UNREACHABLE error("unreachable (%s:%d)", __FILE__, __LINE__)

#endif
