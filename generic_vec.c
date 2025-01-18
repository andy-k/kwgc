// Copyright (C) 2020-2025 Andy Kurnia.

// usage:
// define VEC_ELT_NAME Bool
// define VEC_ELT_T bool
// include
// undef VEC_ELT_T
// undef VEC_ELT_NAME

#define GENERIC_CONCAT_(a, b) a##b
#define GENERIC_CONCAT(a, b) GENERIC_CONCAT_(a, b)

#define VEC_T GENERIC_CONCAT(Vec, VEC_ELT_NAME)
#define VEC_F(suffix) GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(vec, VEC_ELT_NAME), _), suffix)

typedef struct {
  VEC_ELT_T *ptr;
  size_t len, cap;
} VEC_T;

// VecBool v = vecBool_new();
static inline VEC_T VEC_F(new)(void) {
  return (VEC_T){
    .ptr = NULL,
    .len = 0,
    .cap = 0,
  };
}

// vecBool_ensure_cap_exact(&vec, new_cap);
static inline void VEC_F(ensure_cap_exact)(VEC_T self[static 1], size_t new_capacity) {
  if (self->cap < new_capacity) {
    self->ptr = realloc_or_die(self->ptr, new_capacity * sizeof(VEC_ELT_T));
    self->cap = new_capacity;
  }
}

// vecBool_ensure_cap(&vec, min_cap);
static inline void VEC_F(ensure_cap)(VEC_T self[static 1], size_t min_capacity) {
  if (self->cap < min_capacity) {
    size_t new_capacity = min_capacity << 1; // assume no overflow.
    self->ptr = realloc_or_die(self->ptr, new_capacity * sizeof(VEC_ELT_T));
    self->cap = new_capacity;
  }
}

// vecBool_free(&vec);
static inline void VEC_F(free)(VEC_T self[static 1]) {
  free(self->ptr);
  self->ptr = NULL;
  self->len = 0;
  self->cap = 0;
}

// vec_push(&vec, &elt);
static inline void VEC_F(push)(VEC_T self[static 1], VEC_ELT_T *elt) {
  VEC_F(ensure_cap)(self, self->len + 1);
  memcpy(self->ptr + self->len, elt, sizeof(VEC_ELT_T));
  ++self->len;
}

#undef VEC_F
#undef VEC_T
