// Copyright (C) 2020-2024 Andy Kurnia.

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// endian helpers

bool is_big_endian(void) {
  return htons(1) == 1;
}

void swap_bytes_32(uint8_t *p, size_t size_in_bytes) {
  uint8_t t;
  for (size_t i = 3; i < size_in_bytes; i += 4) {
    t = p[i], p[i] = p[i - 3], p[i - 3] = t;
    t = p[i - 1], p[i - 1] = p[i - 2], p[i - 2] = t;
  }
}

// time helpers

bool time_goes_to_stderr = false;

struct timeval now(void) {
  struct timeval tv;
  if (!gettimeofday(&tv, NULL)) return tv;
  return (struct timeval){
    .tv_sec = 0,
    .tv_usec = 0,
  };
}

int fprint_dur_us(FILE *fp, struct timeval tv_end, struct timeval tv_start) {
  int64_t dur_sec = tv_end.tv_sec - tv_start.tv_sec;
  int32_t dur_usec = tv_end.tv_usec - tv_start.tv_usec;
  if (dur_usec < 0) {
    dur_usec += 1000000;
    --dur_sec;
  }
  return fprintf(fp, "%lld.%06d", dur_sec, dur_usec);
}

// malloc helpers

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

// vec helpers

typedef struct {
  uint8_t *ptr;
  size_t len, cap, elt_size;
} vec;

// vec v = vec_new(sizeof(T));
static inline vec vec_new(size_t elt_size) {
  return (vec){
    .ptr = NULL,
    .len = 0,
    .cap = 0,
    .elt_size = elt_size,
  };
}

// vec_ensure_cap_exact(&vec, new_cap);
static inline void vec_ensure_cap_exact(vec self[static 1], size_t new_capacity) {
  if (self->cap < new_capacity) {
    self->ptr = realloc_or_die(self->ptr, new_capacity * self->elt_size);
    self->cap = new_capacity;
  }
}

// vec_ensure_cap(&vec, min_cap);
static inline void vec_ensure_cap(vec self[static 1], size_t min_capacity) {
  if (self->cap < min_capacity) {
    size_t new_capacity = min_capacity * 2; // assume no overflow.
    self->ptr = realloc_or_die(self->ptr, new_capacity * self->elt_size);
    self->cap = new_capacity;
  }
}

// vec_free(&vec);
static inline void vec_free(vec self[static 1]) {
  free(self->ptr);
  self->ptr = NULL;
  self->len = 0;
  self->cap = 0;
}

// *(T *)vec_get(&vec, i)
static inline void *vec_get(vec self[static 1], size_t i) {
  return self->ptr + (i * self->elt_size);
}

// vec_push(&vec, &elt);
static inline void vec_push(vec self[static 1], void *elt) {
  vec_ensure_cap(self, (self->len + 1) * self->elt_size);
  memcpy(vec_get(self, self->len), elt, self->elt_size);
  ++self->len;
}

// kurnia hashmap

typedef struct {
  vec occupieds; // vec<bool>
  vec hashes; // vec<uint64_t>
  vec keys; // vec<K>
  vec values; // vec<V>
  size_t len;
  size_t last_probe;
} khm;

// khm h = khm_new_cap(sizeof(K), sizeof(V), cap);
static inline khm khm_new_cap(size_t key_size, size_t value_size, size_t cap) {
  if (cap != (cap & -cap) || (cap && cap < 16)) { // 0 or power of two of at least 16.
    fprintf(stderr, "invalid cap=%zu\n", cap);
    abort();
  }
  khm ret = {
    .occupieds = vec_new(sizeof(bool)),
    .hashes = vec_new(sizeof(uint64_t)),
    .keys = vec_new(key_size),
    .values = vec_new(value_size),
    .len = 0,
    .last_probe = (size_t)-1,
  };
  vec_ensure_cap_exact(&ret.occupieds, cap);
  vec_ensure_cap_exact(&ret.hashes, cap);
  vec_ensure_cap_exact(&ret.keys, cap);
  vec_ensure_cap_exact(&ret.values, cap);
  memset(ret.occupieds.ptr, 0, cap * sizeof(bool));
  ret.occupieds.len = cap;
  ret.hashes.len = cap;
  ret.keys.len = cap;
  ret.values.len = cap;
  return ret;
}

// khm h = khm_new(sizeof(K), sizeof(V));
static inline khm khm_new(size_t key_size, size_t value_size) {
  return khm_new_cap(key_size, value_size, 16);
}

// khm_free(&khm);
static inline void khm_free(khm self[static 1]) {
  vec_free(&self->values);
  vec_free(&self->keys);
  vec_free(&self->hashes);
  vec_free(&self->occupieds);
  self->len = 0;
}

// size_t idx = khm_locate(&khm, &k, hshk(pk), eqlk);
// returns index where the value is or would end up, or (size_t)-1 if full.
static inline size_t khm_locate(khm self[static 1], void *pk, uint64_t hsh, bool (*eqlk)(void *, void *)) {
  if (!self->occupieds.len) return (size_t)-1;
  size_t mask = self->occupieds.len - 1; // always power of two.
  size_t bucket = hsh & mask;
  size_t probe = bucket;
  do {
    if (!*(bool *)vec_get(&self->occupieds, probe)) return probe;
    if (*(uint64_t *)vec_get(&self->hashes, probe) == hsh && eqlk(vec_get(&self->keys, probe), pk)) return probe;
    probe = (probe + 1) & mask;
  } while (probe != bucket);
  return (size_t)-1;
}

// V *v = khm_get(&khm, &k, hshk, eqlk);
static inline void *khm_get(khm self[static 1], void *pk, uint64_t (*hshk)(void *), bool (*eqlk)(void *, void *)) {
  uint64_t hsh = hshk(pk);
  size_t probe = khm_locate(self, pk, hsh, eqlk);
  self->last_probe = probe;
  if (probe == (size_t)-1) return NULL;
  if (*(bool *)vec_get(&self->occupieds, probe)) return vec_get(&self->values, probe);
  return NULL;
}

// bool inserted = khm_set(&khm, &k, &v, hshk, eqlk);
static inline bool khm_set(khm self[static 1], void *pk, void *pv, uint64_t (*hshk)(void *), bool (*eqlk)(void *, void *)) {
  uint64_t hsh = hshk(pk);
  size_t probe = khm_locate(self, pk, hsh, eqlk);
  if (probe == (size_t)-1) {
    // no space. grow.
    size_t next_cap = self->occupieds.len * 2; // ignore overflow.
    if (next_cap < 16) next_cap = 16;
    khm old = *self;
    // rehash.
    self->occupieds.ptr = NULL;
    self->hashes.ptr = NULL;
    self->keys.ptr = NULL;
    self->values.ptr = NULL;
    vec_ensure_cap_exact(&self->occupieds, next_cap);
    vec_ensure_cap_exact(&self->hashes, next_cap);
    vec_ensure_cap_exact(&self->keys, next_cap);
    vec_ensure_cap_exact(&self->values, next_cap);
    memset(self->occupieds.ptr, 0, next_cap * sizeof(bool));
    self->occupieds.len = next_cap;
    self->hashes.len = next_cap;
    self->keys.len = next_cap;
    self->values.len = next_cap;
    for (size_t i = 0; i < old.occupieds.len; ++i) {
      if (*(bool *)vec_get(&old.occupieds, i)) {
        uint64_t old_hash = *(uint64_t *)vec_get(&old.hashes, i);
        probe = khm_locate(self, vec_get(&old.keys, i), old_hash, eqlk);
        *(bool *)vec_get(&self->occupieds, probe) = true;
        *(uint64_t *)vec_get(&self->hashes, probe) = old_hash;
        memcpy(vec_get(&self->keys, probe), vec_get(&old.keys, i), self->keys.elt_size);
        memcpy(vec_get(&self->values, probe), vec_get(&old.values, i), self->values.elt_size);
      }
    }
    khm_free(&old);
    probe = khm_locate(self, pk, hsh, eqlk);
  }
  self->last_probe = probe;
  if (*(bool *)vec_get(&self->occupieds, probe)) {
    memcpy(vec_get(&self->values, probe), pv, self->values.elt_size);
    return false; // replaced, old value is gone.
  }
  *(bool *)vec_get(&self->occupieds, probe) = true;
  *(uint64_t *)vec_get(&self->hashes, probe) = hsh;
  memcpy(vec_get(&self->keys, probe), pk, self->keys.elt_size);
  memcpy(vec_get(&self->values, probe), pv, self->values.elt_size);
  ++self->len;
  return true; // inserted.
}

// tile-label-specific

#include "tiles.c"

// parser

typedef struct {
  uint32_t ofs; // assume no overflow.
  uint32_t len;
} ofs_len;

// qc = qsort comparator

int qc_chr_cmp(const void *a, const void *b) {
  return memcmp(a, b, 1);
}

uint8_t *qc_ref_tiles_bytes; // temp global, do not free().
int qc_tiles_slices(const void *a, const void *b) {
  ofs_len aol = *(ofs_len *)a;
  ofs_len bol = *(ofs_len *)b;
  uint32_t min_len = aol.len < bol.len ? aol.len : bol.len;
  int mc = memcmp(qc_ref_tiles_bytes + aol.ofs, qc_ref_tiles_bytes + bol.ofs, min_len);
  if (mc) return mc;
  if (min_len < bol.len) return -1;
  if (min_len < aol.len) return 1;
  return 0;
}

static inline bool eql_tiles_slices(void *a, void *b) {
  ofs_len aol = *(ofs_len *)a;
  ofs_len bol = *(ofs_len *)b;
  return aol.len == bol.len && !memcmp(qc_ref_tiles_bytes + aol.ofs, qc_ref_tiles_bytes + bol.ofs, aol.len);
}

// append-only list of words.
typedef struct {
  vec tiles_slices; // vec<ofs_len>
  vec tiles_bytes; // vec<uint8_t>
} wordlist;

static inline wordlist wordlist_new(void) {
  return (wordlist){
      .tiles_slices = vec_new(sizeof(ofs_len)),
      .tiles_bytes = vec_new(sizeof(uint8_t)),
    };
}

static inline void wordlist_free(wordlist self[static 1]) {
  vec_free(&self->tiles_bytes);
  vec_free(&self->tiles_slices);
}

static inline void wordlist_sort(wordlist self[static 1]) {
  qc_ref_tiles_bytes = self->tiles_bytes.ptr;
  qsort(self->tiles_slices.ptr, self->tiles_slices.len, sizeof(ofs_len), qc_tiles_slices);
}

static inline void wordlist_dedup(wordlist self[static 1]) {
  size_t r = 1;
  while (r < self->tiles_slices.len && !eql_tiles_slices(vec_get(&self->tiles_slices, r), vec_get(&self->tiles_slices, r - 1))) ++r;
  if (r < self->tiles_slices.len) {
    // [r] == [r-1]
    size_t w = r;
    while (++r < self->tiles_slices.len) {
      if (!eql_tiles_slices(vec_get(&self->tiles_slices, r), vec_get(&self->tiles_slices, w - 1))) {
        memcpy(vec_get(&self->tiles_slices, w), vec_get(&self->tiles_slices, r), self->tiles_slices.elt_size);
        ++w;
      }
    }
    self->tiles_slices.len = w;
  }
}

// kwg builder

// unconfirmed entries.
typedef struct {
   uint32_t arc_index; // refers to states. should only need 22 bits.
   uint8_t tile;
   bool accepts;
} kwgc_transition;

typedef struct {
  vec transitions; // vec<kwgc_transition>
  vec indexes; // vec<uint32_t>
} kwgc_transition_stack;

static inline kwgc_transition_stack kwgc_transition_stack_new(void) {
  return (kwgc_transition_stack){
    .transitions = vec_new(sizeof(kwgc_transition)),
    .indexes = vec_new(sizeof(uint32_t)),
  };
}

static inline void kwgc_transition_stack_free(kwgc_transition_stack self[static 1]) {
  vec_free(&self->indexes);
  vec_free(&self->transitions);
}

static inline void kwgc_transition_stack_push(kwgc_transition_stack self[static 1], uint8_t tile) {
  vec_push(&self->transitions, &(kwgc_transition){
      .arc_index = 0, // filled up later
      .tile = tile,
      .accepts = false,
    });
  vec_push(&self->indexes, &self->transitions.len);
}

typedef struct {
  uint32_t arc_index; // refers to states.
  uint32_t next_index; // refers to states.
  uint8_t tile;
  bool accepts;
} kwgc_state;

static inline void do_hash(uint64_t *hash, uint8_t data) {
  *hash = (*hash * 3467) ^ (data ^ 0xff);
}

static inline uint64_t kwgc_state_hash(void *p) {
  kwgc_state *self = p;
  uint64_t hash = 0;
  do_hash(&hash, self->tile);
  do_hash(&hash, self->accepts);
  do_hash(&hash, ((uint8_t *)&self->arc_index)[0]);
  do_hash(&hash, ((uint8_t *)&self->arc_index)[1]);
  do_hash(&hash, ((uint8_t *)&self->arc_index)[2]);
  do_hash(&hash, ((uint8_t *)&self->arc_index)[3]);
  do_hash(&hash, ((uint8_t *)&self->next_index)[0]);
  do_hash(&hash, ((uint8_t *)&self->next_index)[1]);
  do_hash(&hash, ((uint8_t *)&self->next_index)[2]);
  do_hash(&hash, ((uint8_t *)&self->next_index)[3]);
  return hash;
}

static inline bool kwgc_state_eql(void *pa, void *pb) {
  kwgc_state *a = pa, *b = pb;
  return a->tile == b->tile &&
    a->accepts == b->accepts &&
    a->arc_index == b->arc_index &&
    a->next_index == b->next_index;
}

typedef struct {
  vec states; // vec<kwgc_state>
  khm states_finder; // khm<kwgc_state,uint32_t>
} kwgc_state_maker;

static inline kwgc_state_maker kwgc_state_maker_new(void) {
  return (kwgc_state_maker){
      .states = vec_new(sizeof(kwgc_state)),
      .states_finder = khm_new(sizeof(kwgc_state), sizeof(uint32_t)),
    };
}

static inline void kwgc_state_maker_free(kwgc_state_maker self[static 1]) {
  khm_free(&self->states_finder);
  vec_free(&self->states);
}

// node_transitions is vec<kwgc_transition>.
static inline uint32_t kwgc_state_maker_make_state(kwgc_state_maker self[static 1], vec node_transitions[static 1], size_t target_len) {
  uint32_t ret = 0;
  for (size_t i = node_transitions->len; i-- > target_len; ) {
    kwgc_transition *node_transition = vec_get(node_transitions, i);
    kwgc_state state = {
        .arc_index = node_transition->arc_index,
        .next_index = ret,
        .tile = node_transition->tile,
        .accepts = node_transition->accepts,
      };
    uint32_t *existing_state_index = khm_get(&self->states_finder, &state, kwgc_state_hash, kwgc_state_eql);
    if (existing_state_index) {
      ret = *existing_state_index;
    } else {
      ret = (uint32_t)self->states.len;
      vec_push(&self->states, &state);
      khm_set(&self->states_finder, &state, &ret, kwgc_state_hash, kwgc_state_eql);
    }
  }
  return ret;
}

static inline void kwgc_transition_stack_pop(kwgc_transition_stack self[static 1], kwgc_state_maker state_maker[static 1]) {
  size_t start_of_batch = (size_t)*(uint32_t *)vec_get(&self->indexes, --self->indexes.len);
  uint32_t new_arc_index = kwgc_state_maker_make_state(state_maker, &self->transitions, start_of_batch);
  ((kwgc_transition *)vec_get(&self->transitions, start_of_batch - 1))->arc_index = new_arc_index;
  self->transitions.len = start_of_batch;
}

static inline uint32_t kwgc_state_maker_make_dawg(kwgc_state_maker self[static 1], wordlist sorted_machine_words[static 1], uint32_t dawg_start_state, bool is_gaddag_phase) {
  kwgc_transition_stack transition_stack = kwgc_transition_stack_new();
  for (size_t machine_word_index = 0; machine_word_index < sorted_machine_words->tiles_slices.len; ++machine_word_index) {
    ofs_len *this_word = vec_get(&sorted_machine_words->tiles_slices, machine_word_index);
    uint32_t prefix_len = 0;
    if (machine_word_index > 0) {
      ofs_len *prev_word = vec_get(&sorted_machine_words->tiles_slices, machine_word_index - 1);
      uint32_t prev_word_len = transition_stack.indexes.len; // this can be one less than prev_word->len for gaddag.
      uint32_t min_word_len = prev_word_len < this_word->len ? prev_word_len : this_word->len;
      while (prefix_len < min_word_len &&
          *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, prev_word->ofs + prefix_len) ==
          *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + prefix_len))
        ++prefix_len;
      for (uint32_t i = prefix_len; i < prev_word_len; ++i) {
        kwgc_transition_stack_pop(&transition_stack, self);
      }
    }
    for (uint32_t i = prefix_len; i < this_word->len; ++i) {
      kwgc_transition_stack_push(&transition_stack, *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + i));
    }
    if (is_gaddag_phase && !*(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + this_word->len - 1)) {
      --transition_stack.indexes.len;
      // gaddag["AC@"] points to dawg["CA"]
      uint32_t p = dawg_start_state;
      for (uint32_t i = this_word->len - 1; i > 0; --i) {
        uint8_t sought_tile = *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + i - 1);
        while (true) {
          kwgc_state *pstate = vec_get(&self->states, p);
          if (pstate->tile == sought_tile) {
            p = pstate->arc_index;
            break;
          }
          p = pstate->next_index;
        }
      }
      ((kwgc_transition *)vec_get(&transition_stack.transitions, transition_stack.transitions.len - 1))->arc_index = p;
    } else {
      ((kwgc_transition *)vec_get(&transition_stack.transitions, transition_stack.transitions.len - 1))->accepts = true;
    }
  }
  while (transition_stack.indexes.len) kwgc_transition_stack_pop(&transition_stack, self);
  uint32_t ret = kwgc_state_maker_make_state(self, &transition_stack.transitions, 0);
  kwgc_transition_stack_free(&transition_stack);
  return ret;
}

typedef struct {
  kwgc_state *states;
  uint32_t *prev_indexes;
  uint32_t *destination;
  uint32_t num_written;
} kwgc_states_defragger;

void kwgc_states_defragger_defrag(kwgc_states_defragger self[static 1], uint32_t p) {
  while (true) {
    uint32_t prev = self->prev_indexes[p];
    if (!prev) break;
    p = prev;
  }
  uint32_t *dp = self->destination + p;
  if (*dp) return;
  // temp value to break self-cycles.
  *dp = (uint32_t)~0;
  uint32_t write_p = p;
  uint32_t num = 0;
  while (true) {
    ++num;
    uint32_t a = self->states[p].arc_index;
    if (a) kwgc_states_defragger_defrag(self, a);
    p = self->states[p].next_index;
    if (!p) break;
  }
  *dp = 0;
  for (uint32_t ofs = 0; ofs < num; ++ofs) {
    // prefer earlier index, so dawg part does not point to gaddag part.
    dp = self->destination + write_p;
    if (*dp) break;
    *dp = self->num_written + ofs;
    write_p = self->states[write_p].next_index;
  }
  // Always += num even if some nodes are necessarily duplicated due to sharing by different prev_nodes.
  self->num_written += num;
}

static inline void kwgc_write_node(uint8_t *pout, uint32_t defragged_arc_index, bool is_end, bool accepts, uint8_t tile) {
  pout[0] = defragged_arc_index;
  pout[1] = defragged_arc_index >> 8;
  pout[2] = ((defragged_arc_index >> 16) & 0x3f) | (uint8_t)(is_end << 6) | (uint8_t)(accepts << 7);
  pout[3] = tile;
}

// ret = vec<uint32_t>, must initially be empty.
void kwgc_build(vec *ret, wordlist sorted_machine_words[static 1], bool is_gaddag) {
  kwgc_state_maker state_maker = kwgc_state_maker_new();
  // The sink state always exists.
  vec_push(&state_maker.states, &(kwgc_state){
      .arc_index = 0,
      .next_index = 0,
      .tile = 0,
      .accepts = false,
    });
  uint32_t zero = 0;
  khm_set(&state_maker.states_finder, vec_get(&state_maker.states, 0), &zero, kwgc_state_hash, kwgc_state_eql);
  uint32_t dawg_start_state = kwgc_state_maker_make_dawg(&state_maker, sorted_machine_words, 0, false);
  uint32_t gaddag_start_state = 0;
  if (is_gaddag) {
    ofs_len cur_ofs_len = { .ofs = 0, .len = 0 };
    wordlist gaddag_wl = wordlist_new();
    for (size_t machine_word_index = 0; machine_word_index < sorted_machine_words->tiles_slices.len; ++machine_word_index) {
      ofs_len *this_word = vec_get(&sorted_machine_words->tiles_slices, machine_word_index);
      uint32_t prefix_len = 0;
      if (machine_word_index > 0) {
        ofs_len *prev_word = vec_get(&sorted_machine_words->tiles_slices, machine_word_index - 1);
        uint32_t max_prefix_len = prev_word->len - 1; // - 1 because CAR -> CARE means we still need to emit RAC@.
        if (this_word->len < max_prefix_len) max_prefix_len = this_word->len;
        while (prefix_len < max_prefix_len &&
            *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, prev_word->ofs + prefix_len) ==
            *(uint8_t *)vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + prefix_len))
          ++prefix_len;
      }
      // CARE = ERAC, RAC@, AC@, C@
      for (size_t i = this_word->len; i > 0; --i) {
        vec_push(&gaddag_wl.tiles_bytes, vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + i - 1));
      }
      cur_ofs_len.len = this_word->len;
      vec_push(&gaddag_wl.tiles_slices, &cur_ofs_len);
      cur_ofs_len.ofs += cur_ofs_len.len;
      uint8_t zero = 0;
      for (size_t j = this_word->len - 1; j > prefix_len; --j) {
        for (size_t i = j; i > 0; --i) {
          vec_push(&gaddag_wl.tiles_bytes, vec_get(&sorted_machine_words->tiles_bytes, this_word->ofs + i - 1));
        }
        vec_push(&gaddag_wl.tiles_bytes, &zero);
        cur_ofs_len.len = j + 1;
        vec_push(&gaddag_wl.tiles_slices, &cur_ofs_len);
        cur_ofs_len.ofs += cur_ofs_len.len;
      }
    }
    wordlist_sort(&gaddag_wl);
    gaddag_start_state = kwgc_state_maker_make_dawg(&state_maker, &gaddag_wl, dawg_start_state, true);
    wordlist_free(&gaddag_wl);
  }
  uint32_t *prev_indexes = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  memset(prev_indexes, 0, state_maker.states.len * sizeof(uint32_t));
  for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
    prev_indexes[((kwgc_state *)vec_get(&state_maker.states, p))->next_index] = p;
  }
  // prev_indexes[0] is garbage, does not matter.
  uint32_t *destination = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  memset(destination, 0, state_maker.states.len * sizeof(uint32_t));
  kwgc_states_defragger states_defragger = {
      .states = (kwgc_state *)state_maker.states.ptr,
      .prev_indexes = prev_indexes,
      .destination = destination,
      .num_written = is_gaddag ? 2 : 1,
    };
  destination[0] = (uint32_t)~0; // useful for empty lexicon.
  kwgc_states_defragger_defrag(&states_defragger, dawg_start_state);
  if (is_gaddag) kwgc_states_defragger_defrag(&states_defragger, gaddag_start_state);
  destination[0] = 0; // useful for empty lexicon.
  if (states_defragger.num_written > 0x400000) {
    // the format can only have 0x400000 elements, each has 4 bytes
    fprintf(stderr, "this format cannot have %u nodes\n", states_defragger.num_written);
    free(destination);
    free(prev_indexes);
    kwgc_state_maker_free(&state_maker);
    return;
  }
  vec_ensure_cap_exact(ret, ret->len = states_defragger.num_written);
  kwgc_write_node((uint8_t *)vec_get(ret, 0), destination[dawg_start_state], true, false, 0);
  if (is_gaddag) kwgc_write_node((uint8_t *)vec_get(ret, 1), destination[gaddag_start_state], true, false, 0);
  for (uint32_t outer_p = 1; outer_p < state_maker.states.len; ++outer_p) {
    if (!prev_indexes[outer_p]) {
      uint32_t dp = destination[outer_p];
      if (dp) {
        for (uint32_t p = outer_p; ; ++dp) {
          kwgc_state *state = states_defragger.states + p;
          kwgc_write_node((uint8_t *)vec_get(ret, dp), destination[state->arc_index], !state->next_index, state->accepts, state->tile);
          if (!state->next_index) break;
          p = state->next_index;
        }
      }
    }
  }
  free(destination);
  free(prev_indexes);
  kwgc_state_maker_free(&state_maker);
}

// commands

bool do_lang_kwg(char **argv, parsed_tile tileset_parse(uint8_t *), int mode) {
  // assume argc >= 4. mode in [0 (dawgonly), 1 (gaddawg), 2 (alpha)].
  bool errored = false;
  bool defer_fclose = false;
  bool defer_free_file_content = false;
  bool defer_free_wl = false;
  bool defer_free_ret = false;
  FILE *f = fopen(argv[2], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t file_size_signed = ftello(f); if (file_size_signed < 0) { perror("ftello"); goto errored; }
  size_t file_size = (size_t)file_size_signed;
  uint8_t *file_content = malloc_or_die(file_size + 1); defer_free_file_content = true;
  rewind(f);
  if (fread(file_content, 1, file_size, f) != file_size) { perror("fread"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  file_content[file_size++] = '\n'; // sentinel
  ofs_len cur_ofs_len = { .ofs = 0, .len = 0 };
  wordlist wl = wordlist_new(); defer_free_wl = true;
  for (size_t i = 0; i < file_size; ) {
    parsed_tile parsed_tile = tileset_parse(file_content + i);
    if (parsed_tile.len && parsed_tile.index > 0) { // ignore blank
      vec_push(&wl.tiles_bytes, &parsed_tile.index);
      i += parsed_tile.len;
      ++cur_ofs_len.len;
    } else if (file_content[i] <= ' ') {
      while (file_content[i] != '\n') ++i;
      ++i; // skip the newline
      if (cur_ofs_len.len > 0) {
        if (mode == 2) qsort(wl.tiles_bytes.ptr + cur_ofs_len.ofs, cur_ofs_len.len, 1, qc_chr_cmp);
        vec_push(&wl.tiles_slices, &cur_ofs_len);
        cur_ofs_len.ofs += cur_ofs_len.len;
        cur_ofs_len.len = 0;
      }
    } else {
      fprintf(stderr, "bad tile at offset %zu\n", i);
      goto errored;
    }
  }
  defer_free_file_content = false; free(file_content);
  wordlist_sort(&wl);
  wordlist_dedup(&wl);
  vec ret = vec_new(sizeof(uint32_t)); defer_free_ret = true;
  kwgc_build(&ret, &wl, mode == 1);
  if (!ret.len) goto errored;
  f = fopen(argv[3], "wb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fwrite(ret.ptr, ret.elt_size, ret.len, f) != ret.len) { perror("fwrite"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_ret) vec_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

bool do_lang_klv2(char **argv, parsed_tile tileset_parse(uint8_t *)) {
  bool errored = false;
  bool defer_fclose = false;
  bool defer_free_file_content = false;
  bool defer_free_wl = false;
  bool defer_free_ret = false;
  bool defer_free_out = false;
  FILE *f = fopen(argv[2], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t file_size_signed = ftello(f); if (file_size_signed < 0) { perror("ftello"); goto errored; }
  size_t file_size = (size_t)file_size_signed;
  uint8_t *file_content = malloc_or_die(file_size + 1); defer_free_file_content = true;
  rewind(f);
  if (fread(file_content, 1, file_size, f) != file_size) { perror("fread"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  file_content[file_size++] = '\n'; // sentinel
  ofs_len cur_ofs_len = { .ofs = 0, .len = 0 };
  wordlist wl = wordlist_new(); defer_free_wl = true;
  bool this_is_big_endian = is_big_endian();
  for (size_t i = 0; i < file_size; ) {
    parsed_tile parsed_tile = tileset_parse(file_content + i);
    if (parsed_tile.len) { // allow blank
      vec_push(&wl.tiles_bytes, &parsed_tile.index);
      i += parsed_tile.len;
      ++cur_ofs_len.len;
    } else if (file_content[i] == ',') {
      size_t orig_i = i;
      while (file_content[i] != '\n') ++i;
      file_content[i] = '\0'; // prevent sscanf from crashing
      float val;
      if (sscanf((char *)(file_content + orig_i + 1), "%f", &val) != 1) {
        fprintf(stderr, "bad value at offset %zu\n", orig_i + 1);
        goto errored;
      }
      ++i; // skip the newline
      if (cur_ofs_len.len > 0) {
        qsort(wl.tiles_bytes.ptr + cur_ofs_len.ofs, cur_ofs_len.len, 1, qc_chr_cmp);
        if (this_is_big_endian) {
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 3);
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 2);
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 1);
          vec_push(&wl.tiles_bytes, &val);
        } else {
          vec_push(&wl.tiles_bytes, &val);
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 1);
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 2);
          vec_push(&wl.tiles_bytes, ((uint8_t *)&val) + 3);
        }
        vec_push(&wl.tiles_slices, &cur_ofs_len);
        cur_ofs_len.ofs += cur_ofs_len.len + 4;
        cur_ofs_len.len = 0;
      }
    } else if (file_content[i] <= ' ' && !cur_ofs_len.len) {
      while (file_content[i] != '\n') ++i;
      ++i; // skip the newline
    } else {
      fprintf(stderr, "bad tile at offset %zu\n", i);
      goto errored;
    }
  }
  defer_free_file_content = false; free(file_content);
  wordlist_sort(&wl);
  wordlist_dedup(&wl);
  vec ret = vec_new(sizeof(uint32_t)); defer_free_ret = true;
  kwgc_build(&ret, &wl, false);
  if (!ret.len) goto errored;
  size_t out_len = ret.len + wl.tiles_slices.len + 2;
  uint8_t *out = malloc_or_die(out_len * 4); defer_free_out = true;
  uint8_t *pout = out;
  *pout++ = ret.len;
  *pout++ = ret.len >> 8;
  *pout++ = ret.len >> 16;
  *pout++ = ret.len >> 24;
  memcpy(pout, ret.ptr, ret.len * ret.elt_size);
  pout += ret.len * ret.elt_size;
  *pout++ = wl.tiles_slices.len;
  *pout++ = wl.tiles_slices.len >> 8;
  *pout++ = wl.tiles_slices.len >> 16;
  *pout++ = wl.tiles_slices.len >> 24;
  for (size_t i = 0; i < wl.tiles_slices.len; ++i) {
    ofs_len *this_word = vec_get(&wl.tiles_slices, i);
    uint8_t *p = vec_get(&wl.tiles_bytes, this_word->ofs + this_word->len);
    *pout++ = *p++;
    *pout++ = *p++;
    *pout++ = *p++;
    *pout++ = *p++;
  }
  f = fopen(argv[3], "wb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fwrite(out, 4, out_len, f) != out_len) { perror("fwrite"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_out) free(out);
  if (defer_free_ret) vec_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

typedef struct { uint32_t p : 22; bool e : 1, d : 1; uint8_t c : 8; } kwg_node; // compiler-specific UB.

void dump_kwg(kwg_node *kwg, vec *word, uint32_t p, tile tileset[static 1]) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t len = orig_len + strlen(tileset[kwg[p].c].label);
    vec_ensure_cap(word, len);
    strcpy(vec_get(word, orig_len), tileset[kwg[p].c].label);
    word->len = len;
    if (kwg[p].d) printf("%.*s\n", (int)len, word->ptr);
    if (kwg[p].p) dump_kwg(kwg, word, kwg[p].p, tileset);
    if (kwg[p].e) break;
  }
  word->len = orig_len;
}

bool do_lang_rkwg(char **argv, tile tileset[static 1]) {
  // assume argc >= 3.
  bool errored = false;
  bool defer_fclose = false;
  bool defer_free_file_content = false;
  bool defer_free_word = false;
  FILE *f = fopen(argv[2], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t file_size_signed = ftello(f); if (file_size_signed < 0) { perror("ftello"); goto errored; }
  size_t file_size = (size_t)file_size_signed;
  uint8_t *file_content = malloc_or_die(file_size); defer_free_file_content = true;
  rewind(f);
  if (fread(file_content, 1, file_size, f) != file_size) { perror("fread"); goto errored; }
  if (is_big_endian()) swap_bytes_32(file_content, file_size);
  kwg_node *kwg = (kwg_node *)file_content;
  vec word = vec_new(sizeof(uint8_t)); defer_free_word = true;
  dump_kwg(kwg, &word, kwg[0].p, tileset);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vec_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

void dump_klv2(kwg_node *kwg, vec *word, uint32_t p, tile tileset[static 1], float **klv_ptr) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t len = orig_len + strlen(tileset[kwg[p].c].label);
    vec_ensure_cap(word, len);
    strcpy(vec_get(word, orig_len), tileset[kwg[p].c].label);
    word->len = len;
    if (kwg[p].d) printf("%.*s,%f\n", (int)len, word->ptr, *(*klv_ptr)++);
    if (kwg[p].p) dump_klv2(kwg, word, kwg[p].p, tileset, klv_ptr);
    if (kwg[p].e) break;
  }
  word->len = orig_len;
}

bool do_lang_rklv2(char **argv, tile tileset[static 1]) {
  // assume argc >= 3.
  bool errored = false;
  bool defer_fclose = false;
  bool defer_free_file_content = false;
  bool defer_free_word = false;
  FILE *f = fopen(argv[2], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t file_size_signed = ftello(f); if (file_size_signed < 0) { perror("ftello"); goto errored; }
  size_t file_size = (size_t)file_size_signed;
  uint8_t *file_content = malloc_or_die(file_size); defer_free_file_content = true;
  rewind(f);
  if (fread(file_content, 1, file_size, f) != file_size) { perror("fread"); goto errored; }
  if (is_big_endian()) swap_bytes_32(file_content, file_size);
  uint32_t num_kwg_nodes = *(uint32_t *)file_content;
  kwg_node *kwg = (kwg_node *)(file_content + 4);
  float *klv_values = (float *)(file_content + 4 * (2 + num_kwg_nodes));
  vec word = vec_new(sizeof(uint8_t)); defer_free_word = true;
  dump_klv2(kwg, &word, kwg[0].p, tileset, &klv_values);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vec_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

bool do_lang(int argc, char **argv, const char lang_name[static 1], parsed_tile tileset_parse(uint8_t *), tile tileset[static 1]) {
  const size_t lang_name_len = strlen(lang_name);
  if (!(argc > 1 && !strncmp(argv[1], lang_name, lang_name_len))) {
    return false;
  } else if (!strcmp(argv[1] + lang_name_len, "-klv2")) {
    if (argc < 4) goto needs_more_args;
    return do_lang_klv2(argv, tileset_parse);
  } else if (!strcmp(argv[1] + lang_name_len, "-kwg")) {
    if (argc < 4) goto needs_more_args;
    return do_lang_kwg(argv, tileset_parse, 1);
  } else if (!strcmp(argv[1] + lang_name_len, "-kwg-alpha")) {
    if (argc < 4) goto needs_more_args;
    return do_lang_kwg(argv, tileset_parse, 2);
  } else if (!strcmp(argv[1] + lang_name_len, "-kwg-dawg")) {
    if (argc < 4) goto needs_more_args;
    return do_lang_kwg(argv, tileset_parse, 0);
  } else if (!strcmp(argv[1] + lang_name_len, "-read-kwg")) {
    if (argc < 3) goto needs_more_args;
    time_goes_to_stderr = true;
    return do_lang_rkwg(argv, tileset);
  } else if (!strcmp(argv[1] + lang_name_len, "-read-klv2")) {
    if (argc < 3) goto needs_more_args;
    time_goes_to_stderr = true;
    return do_lang_rklv2(argv, tileset);
  } else {
    return false;
  }
  return true;
needs_more_args:
  fprintf(stderr, "%s needs more arguments\n", argv[1]);
  return true;
}

int main(int argc, char **argv) {
  struct timeval tv_start = now();
  if (
    do_lang(argc, argv, "english", english_tileset_parse, english_tileset) ||
    do_lang(argc, argv, "catalan", catalan_tileset_parse, catalan_tileset) ||
    do_lang(argc, argv, "french", french_tileset_parse, french_tileset) ||
    do_lang(argc, argv, "german", german_tileset_parse, german_tileset) ||
    do_lang(argc, argv, "norwegian", norwegian_tileset_parse, norwegian_tileset) ||
    do_lang(argc, argv, "polish", polish_tileset_parse, polish_tileset) ||
    do_lang(argc, argv, "slovene", slovene_tileset_parse, slovene_tileset) ||
    do_lang(argc, argv, "spanish", spanish_tileset_parse, spanish_tileset) ||
    do_lang(argc, argv, "yupik", yupik_tileset_parse, yupik_tileset) ||
    false // so newer tilesets can be added without git diff
  ) {
    struct timeval tv_end = now();
    FILE *time_stream = time_goes_to_stderr ? stderr : stdout;
    fputs("time taken: ", time_stream);
    fprint_dur_us(time_stream, tv_end, tv_start);
    fputs("s\n", time_stream);
  } else {
    puts(
      "args:\n"
      "  english-klv2 english.csv english.klv2\n"
      "    generate klv2 file. the csv support is incomplete, no quoting allowed.\n"
      "  english-kwg CSW21.txt CSW21.kwg\n"
      "    generate kwg file containing gaddawg\n"
      "  english-kwg-alpha CSW21.txt CSW21.kad\n"
      "    generate kad file containing alpha dawg\n"
      "  english-kwg-dawg CSW21.txt outfile.dwg\n"
      "    generate dawg-only file\n"
      "  english-read-kwg outfile.kwg\n"
      "    read kwg on a little-endian system\n"
      "  english-read-klv2 outfile.klv2\n"
      "    read klv2 on a little-endian system\n"
      "  (english can also be catalan, french, german, norwegian, polish, slovene,\n"
      "    spanish, yupik)");
  }
}