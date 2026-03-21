// Copyright (C) 2020-2026 Andy Kurnia.

static inline void *not_null_or_die(void *ptr) {
  if (!ptr) { perror("not_null_or_die"); abort(); }
  return ptr;
}

static inline void *malloc_or_die(size_t size) {
  return not_null_or_die(malloc(size));
}

static inline void *realloc_or_die(void *ptr, size_t size) {
  return not_null_or_die(realloc(ptr, size));
}
