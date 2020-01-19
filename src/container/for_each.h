#ifndef OIR_CONTAINER_FOR_EACH_H
#define OIR_CONTAINER_FOR_EACH_H

// foreach macros.
// NOTE that `continue` works properly in these macros, but `break` doesn't.

#define FOR_EACH(T, item, Name, list)                                                              \
  for (bool keep = 1; keep; keep = !keep)                                                          \
    for (Name##Iterator* it_##item = front_##Name(list);                                           \
         keep && !is_nil_##Name##Iterator(it_##item);                                              \
         keep = !keep, it_##item = next_##Name##Iterator(it_##item))                               \
      for (T item = data_##Name##Iterator(it_##item); keep; keep = !keep)

#define FOR_EACH_BACK(T, item, Name, list)                                                         \
  for (bool keep = 1; keep; keep = !keep)                                                          \
    for (Name##Iterator* it_##item = back_##Name(list);                                            \
         keep && !is_nil_##Name##Iterator(it_##item);                                              \
         keep = !keep, it_##item = prev_##Name##Iterator(it_##item))                               \
      for (T item = data_##Name##Iterator(it_##item); keep; keep = !keep)

#define FOR_EACH_VEC(T, item, Name, vec)                                                           \
  for (bool keep = 1; keep; keep = !keep)                                                          \
    for (unsigned i_##item = 0; keep && i_##item < length_##Name(vec); keep = !keep, i_##item++)   \
      for (T item = get_##Name(vec, i_##item); keep; keep = !keep)

#endif
