#ifndef OIR_CONTAINER_BIT_SET_H
#define OIR_CONTAINER_BIT_SET_H

#include <stdbool.h>
#include <stdio.h>

typedef struct BitSet BitSet;

BitSet* new_BitSet(unsigned length);
unsigned length_BitSet(const BitSet*);
void resize_BitSet(BitSet*, unsigned length);

void or_BitSet(BitSet*, const BitSet*);
void and_BitSet(BitSet*, const BitSet*);
void diff_BitSet(BitSet*, const BitSet*);

bool get_BitSet(const BitSet*, unsigned);
void set_BitSet(BitSet*, unsigned, bool);

void clear_BitSet(BitSet*);
void fill_BitSet(BitSet*);
BitSet* copy_BitSet(const BitSet*);
void copy_to_BitSet(BitSet*, const BitSet*);
bool equal_to_BitSet(const BitSet*, const BitSet*);
unsigned count_BitSet(const BitSet*);
unsigned mssb_BitSet(const BitSet*);

void print_BitSet(FILE*, const BitSet*);
void release_BitSet(BitSet*);

#endif
