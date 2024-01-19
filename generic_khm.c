// Copyright (C) 2020-2024 Andy Kurnia.

// kurnia hashmap

// usage:

// define Vec for Bool, U64, K, T.
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

#define GENERIC_CONCAT_(a, b) a##b
#define GENERIC_CONCAT(a, b) GENERIC_CONCAT_(a, b)

#define KHM_T GENERIC_CONCAT(GENERIC_CONCAT(Khm, KHM_K_NAME), KHM_V_NAME)
#define KHM_F(suffix) GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(khm, KHM_K_NAME), KHM_V_NAME), _), suffix)

#define KHM_VEC_K_T GENERIC_CONCAT(Vec, KHM_K_NAME)
#define KHM_VEC_K_F(suffix) GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(vec, KHM_K_NAME), _), suffix)
#define KHM_VEC_V_T GENERIC_CONCAT(Vec, KHM_V_NAME)
#define KHM_VEC_V_F(suffix) GENERIC_CONCAT(GENERIC_CONCAT(GENERIC_CONCAT(vec, KHM_V_NAME), _), suffix)

typedef struct {
  VecBool occupieds;
  VecU64 hashes;
  KHM_VEC_K_T keys;
  KHM_VEC_V_T values;
  size_t len;
  size_t last_probe; // note: this would be useful if caller needs to strdup()
} KHM_T;

// KhmKV h = khmKV_new_cap(cap);
static inline KHM_T KHM_F(new_cap)(size_t cap) {
  if (cap != (cap & -cap) || cap < 16) { // power of two of at least 16.
    fprintf(stderr, "invalid cap=%zu\n", cap);
    abort();
  }
  KHM_T ret = {
    .occupieds = vecBool_new(),
    .hashes = vecU64_new(),
    .keys = KHM_VEC_K_F(new)(),
    .values = KHM_VEC_V_F(new)(),
    .len = 0,
    .last_probe = (size_t)-1,
  };
  vecBool_ensure_cap_exact(&ret.occupieds, cap);
  vecU64_ensure_cap_exact(&ret.hashes, cap);
  KHM_VEC_K_F(ensure_cap_exact)(&ret.keys, cap);
  KHM_VEC_V_F(ensure_cap_exact)(&ret.values, cap);
  memset(ret.occupieds.ptr, 0, cap * sizeof(bool));
  ret.occupieds.len = cap;
  ret.hashes.len = cap;
  ret.keys.len = cap;
  ret.values.len = cap;
  return ret;
}

// KhmKV h = khmKV_new();
static inline KHM_T KHM_F(new)(void) {
  return KHM_F(new_cap)(16);
}

// khmKV_free(&khm);
static inline void KHM_F(free)(KHM_T self[static 1]) {
  KHM_VEC_V_F(free)(&self->values);
  KHM_VEC_K_F(free)(&self->keys);
  vecU64_free(&self->hashes);
  vecBool_free(&self->occupieds);
  self->len = 0;
}

// size_t idx = khmKV_locate(&khm, &k, hshk(pk));
// returns index where the value is or would end up, or (size_t)-1 if full.
static inline size_t KHM_F(locate)(KHM_T self[static 1], KHM_K_T *pk, uint64_t hsh) {
  size_t mask = self->occupieds.len - 1; // always power of two.
  size_t bucket = hsh & mask;
  size_t probe = bucket;
  do {
    if (!self->occupieds.ptr[probe]) return probe;
    if (self->hashes.ptr[probe] == hsh && KHM_K_EQLFUNC(&self->keys.ptr[probe], pk)) return probe;
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
  if (self->occupieds.ptr[probe]) return self->values.ptr + probe;
  return NULL;
}

// bool inserted = khmKV_set(&khm, &k, &v);
static inline bool KHM_F(set)(KHM_T self[static 1], KHM_K_T *pk, KHM_V_T *pv) {
  uint64_t hsh = KHM_K_HASHFUNC(pk);
  size_t probe = self->len + self->len / 3 >= self->occupieds.len ? (size_t)-1 : KHM_F(locate)(self, pk, hsh);
  if (probe == (size_t)-1) {
    // no space. grow.
    size_t next_cap = self->occupieds.len << 1; // ignore overflow.
    KHM_T old = *self;
    // rehash.
    self->occupieds.ptr = NULL;
    self->hashes.ptr = NULL;
    self->keys.ptr = NULL;
    self->values.ptr = NULL;
    vecBool_ensure_cap_exact(&self->occupieds, next_cap);
    vecU64_ensure_cap_exact(&self->hashes, next_cap);
    KHM_VEC_K_F(ensure_cap_exact)(&self->keys, next_cap);
    KHM_VEC_V_F(ensure_cap_exact)(&self->values, next_cap);
    memset(self->occupieds.ptr, 0, next_cap * sizeof(bool));
    self->occupieds.len = next_cap;
    self->hashes.len = next_cap;
    self->keys.len = next_cap;
    self->values.len = next_cap;
    for (size_t i = 0; i < old.occupieds.len; ++i) {
      if (old.occupieds.ptr[i]) {
        uint64_t old_hash = old.hashes.ptr[i];
        probe = KHM_F(locate)(self, old.keys.ptr + i, old_hash);
        self->occupieds.ptr[probe] = true;
        self->hashes.ptr[probe] = old_hash;
        memcpy(self->keys.ptr + probe, old.keys.ptr + i, sizeof(KHM_K_T));
        memcpy(self->values.ptr + probe, old.values.ptr + i, sizeof(KHM_V_T));
      }
    }
    KHM_F(free)(&old);
    probe = KHM_F(locate)(self, pk, hsh);
  }
  self->last_probe = probe;
  if (self->occupieds.ptr[probe]) {
    memcpy(self->values.ptr + probe, pv, sizeof(KHM_V_T));
    return false; // replaced, old value is gone.
  }
  self->occupieds.ptr[probe] = true;
  self->hashes.ptr[probe] = hsh;
  memcpy(self->keys.ptr + probe, pk, sizeof(KHM_K_T));
  memcpy(self->values.ptr + probe, pv, sizeof(KHM_V_T));
  ++self->len;
  return true; // inserted.
}

#undef KHM_VEC_V_F
#undef KHM_VEC_V_T
#undef KHM_VEC_K_F
#undef KHM_VEC_K_T

#undef KHM_F
#undef KHM_T
