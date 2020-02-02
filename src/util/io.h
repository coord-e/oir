#ifndef OIR_UTIL_IO_H
#define OIR_UTIL_IO_H

#include <stdio.h>

FILE* open_file(const char* path, const char* mode);
void close_file(FILE*, const char* path);
char* read_file(const char* path);
void write_file(const char* path, const char* content);

#endif
