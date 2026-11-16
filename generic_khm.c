// Copyright (C) 2020-2026 Andy Kurnia.

// kurnia hashmap

// usage:

// implement uint64_t hashFunc(KHM_K_T*).
// implement bool eqlFunc(KHM_K_T*, KHM_K_T*).

// define KHM_K_NAME Bool
// define KHM_K_T bool
// define KHM_K_HASHFUNC hshBool
// define KHM_K_EQLFUNC eqlBool
// define KHM_V_NAME U64
// define KHM_V_T uint64_t
// include
// undef KHM_V_T
// undef KHM_V_NAME
// undef KHM_K_EQLFUNC
// undef KHM_K_HASHFUNC
// undef KHM_K_T
// undef KHM_K_NAME

// an open-addressing table, probed linearly, of a power of two slots kept at
// most three quarters full. each slot holds its key and value, so a probe
// reads no other memory. a hash's top bits, after Fibonacci hashing, pick
// its slot.

#define GENERIC_CONCAT_(a, b) a##b
#define GENERIC_CONCAT(a, b) GENERIC_CONCAT_(a, b)

// Fibonacci hashing's multiplier, 2^64 divided by the golden ratio.
#define KHM_FIBONACCI_MULTIPLIER UINT64_C(0x9e3779b97f4a7c15)

#define KHM_T GENERIC_CONCAT(GENERIC_CONCAT(Khm, KHM_K_NAME), KHM_V_NAME)
#define KHM_SLOT_T GENERIC_CONCAT(KHM_T, Slot)
#define KHM_F(suffix) GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(khm, KHM_K_NAME), KHM_V_NAME), _), suffix)

typedef struct {
  KHM_K_T key;
  KHM_V_T value;
  bool occupied;
} KHM_SLOT_T;

typedef struct {
  KHM_SLOT_T *slots;
  size_t cap; // always power of two.
  unsigned shift; // 64 - log2(cap).
  size_t len;
  size_t last_probe; // note: this would be useful if caller needs to strdup()
} KHM_T;

static inline KHM_SLOT_T *KHM_F(alloc_slots)(size_t cap) {
  KHM_SLOT_T *slots = malloc_or_die(cap * sizeof(KHM_SLOT_T));
  for (size_t i = 0; i < cap; ++i) slots[i].occupied = false;
  return slots;
}

// KhmKV h = khmKV_new_cap(cap);
static inline KHM_T KHM_F(new_cap)(size_t cap) {
  if (cap != (cap & -cap) || cap < 16) { // power of two of at least 16.
    fprintf(stderr, "invalid cap=%zu\n", cap);
    abort();
  }
  unsigned log2_cap = 0;
  while (((size_t)1 << log2_cap) < cap) ++log2_cap;
  return (KHM_T){
    .slots = KHM_F(alloc_slots)(cap),
    .cap = cap,
    .shift = 64 - log2_cap,
    .len = 0,
    .last_probe = (size_t)-1,
  };
}

// KhmKV h = khmKV_new();
static inline KHM_T KHM_F(new)(void) {
  return KHM_F(new_cap)(16);
}

// KhmKV h = khmKV_new_for_len(expected_len);
// room for expected_len keys before it grows.
static inline KHM_T KHM_F(new_for_len)(size_t expected_len) {
  size_t cap = 16;
  while (cap - cap / 4 < expected_len) cap <<= 1; // ignore overflow.
  return KHM_F(new_cap)(cap);
}

// khmKV_free(&khm);
static inline void KHM_F(free)(KHM_T self[static 1]) {
  free(self->slots);
  self->slots = NULL;
  self->len = 0;
}

static inline size_t KHM_F(bucket)(KHM_T self[static 1], uint64_t hsh) {
  return (size_t)((hsh * KHM_FIBONACCI_MULTIPLIER) >> self->shift);
}

// size_t idx = khmKV_locate(&khm, &k, hshk(pk));
// returns index where the value is or would end up, or (size_t)-1 if full.
static inline size_t KHM_F(locate)(KHM_T self[static 1], KHM_K_T *pk, uint64_t hsh) {
  size_t mask = self->cap - 1;
  size_t bucket = KHM_F(bucket)(self, hsh);
  size_t probe = bucket;
  do {
    KHM_SLOT_T *slot = self->slots + probe;
    if (!slot->occupied) return probe;
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    if (KHM_K_EQLFUNC(&slot->key, pk)) return probe;
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
    probe = (probe + 1) & mask;
  } while (probe != bucket);
  return (size_t)-1;
}

// V *v = khmKV_get(&khm, &k);
static inline KHM_V_T *KHM_F(get)(KHM_T self[static 1], KHM_K_T *pk) {
  uint64_t hsh = KHM_K_HASHFUNC(pk);
  size_t probe = KHM_F(locate)(self, pk, hsh);
  self->last_probe = probe;
  if (probe == (size_t)-1) return NULL;
  if (self->slots[probe].occupied) return &self->slots[probe].value;
  return NULL;
}

// twice the slots, every key rehashed into them.
static void KHM_F(grow)(KHM_T self[static 1]) {
  KHM_T old = *self;
  self->cap <<= 1; // ignore overflow.
  --self->shift;
  self->slots = KHM_F(alloc_slots)(self->cap);
  size_t mask = self->cap - 1;
  for (size_t i = 0; i < old.cap; ++i) {
    KHM_SLOT_T *old_slot = old.slots + i;
    if (old_slot->occupied) {
      size_t probe = KHM_F(bucket)(self, KHM_K_HASHFUNC(&old_slot->key));
      while (self->slots[probe].occupied) probe = (probe + 1) & mask;
      memcpy(self->slots + probe, old_slot, sizeof(KHM_SLOT_T));
    }
  }
  KHM_F(free)(&old);
}

// the slot k is or would be in, with room for it.
static inline size_t KHM_F(locate_for_set)(KHM_T self[static 1], KHM_K_T *pk, uint64_t hsh) {
  size_t probe = self->len + self->len / 3 >= self->cap ? (size_t)-1 : KHM_F(locate)(self, pk, hsh);
  if (probe == (size_t)-1) {
    // no space. grow.
    KHM_F(grow)(self);
    probe = KHM_F(locate)(self, pk, hsh);
  }
  return probe;
}

// bool inserted = khmKV_set(&khm, &k, &v);
static inline bool KHM_F(set)(KHM_T self[static 1], KHM_K_T *pk, KHM_V_T *pv) {
  uint64_t hsh = KHM_K_HASHFUNC(pk);
  size_t probe = KHM_F(locate_for_set)(self, pk, hsh);
  self->last_probe = probe;
  KHM_SLOT_T *slot = self->slots + probe;
  if (slot->occupied) {
    memcpy(&slot->value, pv, sizeof(KHM_V_T));
    return false; // replaced, old value is gone.
  }
  slot->occupied = true;
  memcpy(&slot->key, pk, sizeof(KHM_K_T));
  memcpy(&slot->value, pv, sizeof(KHM_V_T));
  ++self->len;
  return true; // inserted.
}

// V *v = khmKV_get_or_set(&khm, &k, &v);
// the value k has, else v, now k's: one probe either way.
static inline KHM_V_T *KHM_F(get_or_set)(KHM_T self[static 1], KHM_K_T *pk, KHM_V_T *pv) {
  uint64_t hsh = KHM_K_HASHFUNC(pk);
  size_t probe = KHM_F(locate_for_set)(self, pk, hsh);
  self->last_probe = probe;
  KHM_SLOT_T *slot = self->slots + probe;
  if (!slot->occupied) {
    slot->occupied = true;
    memcpy(&slot->key, pk, sizeof(KHM_K_T));
    memcpy(&slot->value, pv, sizeof(KHM_V_T));
    ++self->len;
  }
  return &slot->value;
}

#undef KHM_F
#undef KHM_SLOT_T
#undef KHM_T
