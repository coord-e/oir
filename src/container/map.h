#ifndef OIR_CONTAINER_MAP_H
#define OIR_CONTAINER_MAP_H

#include <stdio.h>

#include "container/list.h"
#include "container/vector.h"
#include "util.h"

// hash table from string to `T`
#define DECLARE_MAP(T, Name)                                                                       \
  typedef struct Name Name;                                                                        \
  Name* new_##Name(unsigned size);                                                                 \
  Name* copy_##Name(const Name*);                                                                  \
  Name* shallow_copy_##Name(const Name*);                                                          \
  void insert_##Name(Name*, const char* k, T v);                                                   \
  T get_##Name(Name*, const char* k);                                                              \
  bool lookup_##Name(Name*, const char* k, T* out);                                                \
  void remove_##Name(Name*, const char* k);                                                        \
  void release_##Name(Name*);

#define DEFINE_MAP(copy_T, release_T, T, Name)                                                     \
  typedef struct {                                                                                 \
    unsigned hash;                                                                                 \
    char* key;                                                                                     \
    T value;                                                                                       \
  } Name##Entry;                                                                                   \
  static void release_##Name##Entry(Name##Entry* e) {                                              \
    if (e == NULL) {                                                                               \
      return;                                                                                      \
    }                                                                                              \
    free(e->key);                                                                                  \
    release_T(e->value);                                                                           \
    free(e);                                                                                       \
  }                                                                                                \
  DECLARE_LIST(Name##Entry*, Name##Entries)                                                        \
  DECLARE_VECTOR(Name##Entries*, Name##Table)                                                      \
  DEFINE_LIST(release_##Name##Entry, Name##Entry*, Name##Entries)                                  \
  DEFINE_VECTOR(release_##Name##Entries, Name##Entries*, Name##Table)                              \
  struct Name {                                                                                    \
    Name##Table* table;                                                                            \
  };                                                                                               \
  Name* new_##Name(unsigned size) {                                                                \
    Name* m  = calloc(1, sizeof(Name));                                                            \
    m->table = new_##Name##Table(size);                                                            \
    for (unsigned i = 0; i < size; i++) {                                                          \
      push_##Name##Table(m->table, new_##Name##Entries());                                         \
    }                                                                                              \
    return m;                                                                                      \
  }                                                                                                \
  static Name##Entry* new_##Name##Entry() { return calloc(1, sizeof(Name##Entry)); }               \
  static Name##Entry* copy_##Name##Entry(Name##Entry* e, bool copy_value) {                        \
    Name##Entry* new = new_##Name##Entry();                                                        \
    new->hash        = e->hash;                                                                    \
    new->key         = strdup(e->key);                                                             \
    if (copy_value) {                                                                              \
      new->value = copy_T(e->value);                                                               \
    } else {                                                                                       \
      new->value = e->value;                                                                       \
    }                                                                                              \
    return new;                                                                                    \
  }                                                                                                \
  static Name##Entries* copy_entries_##Name(Name##Entries* list, bool copy_value) {                \
    Name##Entries* copy = new_##Name##Entries();                                                   \
    for (Name##Entries##Iterator* it               = front_##Name##Entries(list);                  \
         !is_nil_##Name##Entries##Iterator(it); it = next_##Name##Entries##Iterator(it)) {         \
      Name##Entry* c = data_##Name##Entries##Iterator(it);                                         \
      Name##Entry* e = copy_##Name##Entry(c, copy_value);                                          \
      push_back_##Name##Entries(copy, e);                                                          \
    }                                                                                              \
    return copy;                                                                                   \
  }                                                                                                \
  static Name* copy_impl_##Name(const Name* m, bool is_deep) {                                     \
    Name* copy    = calloc(1, sizeof(Name));                                                       \
    unsigned size = length_##Name##Table(m->table);                                                \
    copy->table   = new_##Name##Table(size);                                                       \
    for (unsigned i = 0; i < size; i++) {                                                          \
      Name##Entries* l = get_##Name##Table(m->table, i);                                           \
      push_##Name##Table(copy->table, copy_entries_##Name(l, is_deep));                            \
    }                                                                                              \
    return copy;                                                                                   \
  }                                                                                                \
  Name* shallow_copy_##Name(const Name* m) { return copy_impl_##Name(m, false); }                  \
  Name* copy_##Name(const Name* m) { return copy_impl_##Name(m, true); }                           \
  static Name##Entry* make_entry_##Name(const char* k, T v) {                                      \
    unsigned hash  = hash_string(k);                                                               \
    Name##Entry* e = new_##Name##Entry();                                                          \
    e->hash        = hash;                                                                         \
    e->value       = v;                                                                            \
    e->key         = strdup(k);                                                                    \
    return e;                                                                                      \
  }                                                                                                \
  void insert_##Name(Name* m, const char* k, T v) {                                                \
    Name##Entry* e    = make_entry_##Name(k, v);                                                   \
    unsigned idx      = e->hash % length_##Name##Table(m->table);                                  \
    Name##Entries* es = get_##Name##Table(m->table, idx);                                          \
    push_front_##Name##Entries(es, e);                                                             \
  }                                                                                                \
  T get_##Name(Name* m, const char* k) {                                                           \
    T out;                                                                                         \
    if (lookup_##Name(m, k, &out)) {                                                               \
      return out;                                                                                  \
    }                                                                                              \
    error("key \"%s\" not found", k);                                                              \
  }                                                                                                \
  static bool search_##Name(Name##Entries* es, unsigned hash, T* out) {                            \
    for (Name##Entries##Iterator* it               = front_##Name##Entries(es);                    \
         !is_nil_##Name##Entries##Iterator(it); it = next_##Name##Entries##Iterator(it)) {         \
      Name##Entry* e = data_##Name##Entries##Iterator(it);                                         \
      if (e->hash == hash) {                                                                       \
        if (out != NULL) {                                                                         \
          *out = e->value;                                                                         \
        }                                                                                          \
        return true;                                                                               \
      }                                                                                            \
    }                                                                                              \
    return false;                                                                                  \
  }                                                                                                \
  bool lookup_##Name(Name* m, const char* k, T* out) {                                             \
    unsigned hash     = hash_string(k);                                                            \
    unsigned idx      = hash % length_##Name##Table(m->table);                                     \
    Name##Entries* es = get_##Name##Table(m->table, idx);                                          \
    return search_##Name(es, hash, out);                                                           \
  }                                                                                                \
  static void search_remove_##Name(Name##Entries* es, unsigned hash) {                             \
    for (Name##Entries##Iterator* it               = front_##Name##Entries(es);                    \
         !is_nil_##Name##Entries##Iterator(it); it = next_##Name##Entries##Iterator(it)) {         \
      Name##Entry* e = data_##Name##Entries##Iterator(it);                                         \
      if (e->hash == hash) {                                                                       \
        remove_##Name##Entries##Iterator(it);                                                      \
        return;                                                                                    \
      }                                                                                            \
    }                                                                                              \
    return;                                                                                        \
  }                                                                                                \
  void remove_##Name(Name* m, const char* k) {                                                     \
    unsigned hash     = hash_string(k);                                                            \
    unsigned idx      = hash % length_##Name##Table(m->table);                                     \
    Name##Entries* es = get_##Name##Table(m->table, idx);                                          \
    search_remove_##Name(es, hash);                                                                \
  }                                                                                                \
  void release_##Name(Name* m) {                                                                   \
    if (m == NULL) {                                                                               \
      return;                                                                                      \
    }                                                                                              \
    release_##Name##Table(m->table);                                                               \
  }

#define DECLARE_MAP_PRINTER(Name) void print_##Name(FILE*, Name*);

#define DEFINE_MAP_PRINTER(print_T, begin, sep, sep_kv, end, Name)                                 \
  static void print_entries_##Name(FILE* p, Name##Entries* es) {                                   \
    for (Name##Entries##Iterator* it               = front_##Name##Entries(es);                    \
         !is_nil_##Name##Entries##Iterator(it); it = next_##Name##Entries##Iterator(it)) {         \
      Name##Entry* e = data_##Name##Entries##Iterator(it);                                         \
      fputs(e->key, p);                                                                            \
      fputs(sep_kv, p);                                                                            \
      print_T(p, e->value);                                                                        \
      if (!is_nil_##Name##Entries##Iterator(next_##Name##Entries##Iterator(it))) {                 \
        fputs(sep, p);                                                                             \
      }                                                                                            \
    }                                                                                              \
  }                                                                                                \
  void print_##Name(FILE* p, Name* m) {                                                            \
    fputs(begin, p);                                                                               \
    for (unsigned i = 0; i < length_##Name##Table(m->table); i++) {                                \
      print_entries_##Name(p, get_##Name##Table(m->table, i));                                     \
    }                                                                                              \
    fputs(end, p);                                                                                 \
  }

#endif
