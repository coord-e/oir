#include "util/dummy.h"

void dummy_release(void* x) {}
void dummy_release_int(long long x) {}
void* dummy_copy(void* x) {
  return x;
}
