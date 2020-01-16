#include "container/bit_set.h"

#include <stdint.h>

#include "container/vector.h"
#include "util.h"

DECLARE_VECTOR(uint64_t, U64Vec)
static void release_u64(uint64_t l) {}
DEFINE_VECTOR(release_u64, uint64_t, U64Vec)

struct BitSet {
  U64Vec* data;
  unsigned length;
};

const size_t block_size = sizeof(uint64_t) * 8;

static BitSet* init_BitSet() {
  return calloc(1, sizeof(BitSet));
}

static unsigned calc_size(unsigned length) {
  return (length + block_size - 1) / block_size;
}

BitSet* new_BitSet(unsigned length) {
  BitSet* s     = init_BitSet();
  unsigned size = calc_size(length);
  s->data       = new_U64Vec(size);
  resize_U64Vec(s->data, size);
  s->length = length;
  clear_BitSet(s);
  return s;
}

unsigned length_BitSet(const BitSet* s) {
  return s->length;
}

void resize_BitSet(BitSet* s, unsigned length) {
  unsigned size = calc_size(length);
  resize_U64Vec(s->data, size);
  s->length = length;
}

void or_BitSet(BitSet* s1, const BitSet* s2) {
  assert(s1->length == s2->length);
  for (unsigned i = 0; i < length_U64Vec(s1->data); i++) {
    uint64_t d1 = get_U64Vec(s1->data, i);
    uint64_t d2 = get_U64Vec(s2->data, i);
    set_U64Vec(s1->data, i, d1 | d2);
  }
}

void and_BitSet(BitSet* s1, const BitSet* s2) {
  assert(s1->length == s2->length);
  for (unsigned i = 0; i < length_U64Vec(s1->data); i++) {
    uint64_t d1 = get_U64Vec(s1->data, i);
    uint64_t d2 = get_U64Vec(s2->data, i);
    set_U64Vec(s1->data, i, d1 & d2);
  }
}

void diff_BitSet(BitSet* s1, const BitSet* s2) {
  assert(s1->length == s2->length);
  for (unsigned i = 0; i < length_U64Vec(s1->data); i++) {
    uint64_t d1 = get_U64Vec(s1->data, i);
    uint64_t d2 = get_U64Vec(s2->data, i);
    set_U64Vec(s1->data, i, d1 & (~d2));
  }
}

bool get_BitSet(const BitSet* s, unsigned idx) {
  uint64_t data = get_U64Vec(s->data, idx / block_size);
  unsigned pos  = idx % block_size;
  return (data >> pos) & UINT64_C(1);
}

void set_BitSet(BitSet* s, unsigned idx, bool b) {
  uint64_t data = get_U64Vec(s->data, idx / block_size);
  unsigned pos  = idx % block_size;
  if (b) {
    data |= UINT64_C(1) << pos;
  } else {
    data &= ~(UINT64_C(1) << pos);
  }
  set_U64Vec(s->data, idx / block_size, data);
}

void clear_BitSet(BitSet* s) {
  for (unsigned i = 0; i < length_U64Vec(s->data); i++) {
    set_U64Vec(s->data, i, 0);
  }
}

void fill_BitSet(BitSet* s) {
  for (unsigned i = 0; i < length_U64Vec(s->data); i++) {
    set_U64Vec(s->data, i, 0xFFFFFFFF);
  }
}

BitSet* copy_BitSet(const BitSet* s) {
  BitSet* new = init_BitSet();
  new->length = s->length;
  new->data   = shallow_copy_U64Vec(s->data);
  return new;
}

void copy_to_BitSet(BitSet* s1, const BitSet* s2) {
  assert(s1->length == s2->length);
  for (unsigned i = 0; i < length_U64Vec(s1->data); i++) {
    uint64_t d = get_U64Vec(s2->data, i);
    set_U64Vec(s1->data, i, d);
  }
}

bool equal_to_BitSet(const BitSet* s1, const BitSet* s2) {
  if (s1->length != s2->length) {
    return false;
  }

  for (unsigned i = 0; i < length_U64Vec(s1->data); i++) {
    uint64_t d1 = get_U64Vec(s1->data, i);
    uint64_t d2 = get_U64Vec(s2->data, i);
    if (d1 != d2) {
      return false;
    }
  }

  return true;
}

unsigned count_BitSet(const BitSet* s) {
  unsigned res = 0;
  for (unsigned i = 0; i < length_U64Vec(s->data); i++) {
    uint64_t d = get_U64Vec(s->data, i);
    // TODO: faster count
    for (; d != 0; d &= d - 1) {
      res++;
    }
  }
  return res;
}

// most significant set bit
unsigned mssb_BitSet(const BitSet* s) {
  assert(count_BitSet(s) != 0);
  for (unsigned j = length_U64Vec(s->data); j > 0; j--) {
    unsigned i = j - 1;
    uint64_t d = get_U64Vec(s->data, i);
    if (d == 0) {
      continue;
    }

    // TODO: faster count
    unsigned mssb = 0;
    while (d) {
      d >>= 1;
      mssb++;
    }

    return mssb - 1 + i * block_size;
  }
  OIR_UNREACHABLE;
}

void print_BitSet(FILE* p, const BitSet* s) {
  fputs("{", p);
  for (unsigned i = 0; i < s->length; i++) {
    if (get_BitSet(s, i)) {
      fprintf(p, "%d, ", i);
    }
  }
  fputs("}", p);
}

void release_BitSet(BitSet* s) {
  if (s == NULL) {
    return;
  }

  release_U64Vec(s->data);
  free(s);
}
