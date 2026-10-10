// Copyright (C) 2020-2026 Andy Kurnia.

#include <arpa/inet.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

typedef enum {
    BuildLayout_Legacy,
    BuildLayout_Magpie,
    BuildLayout_MagpieMerged,
    BuildLayout_Experimental,
    BuildLayout_Wolges,
} BuildLayout;

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
  return fprintf(fp, "%" PRId64 ".%06d", dur_sec, dur_usec);
}

// malloc helpers

#include "alloc.c"

// generic vec types

#define VEC_ELT_NAME Bool
#define VEC_ELT_T bool
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

#define VEC_ELT_NAME U64
#define VEC_ELT_T uint64_t
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

#define VEC_ELT_NAME Byte
#define VEC_ELT_T uint8_t
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

#define VEC_ELT_NAME U32
#define VEC_ELT_T uint32_t
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

#define VEC_ELT_NAME Char
#define VEC_ELT_T char
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

// tile-label-specific

#include "tiles.c"

// parser

typedef struct {
  uint32_t ofs; // assume no overflow.
  uint32_t len;
} OfsLen;

#define VEC_ELT_NAME OfsLen
#define VEC_ELT_T OfsLen
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

// qc = qsort comparator

int qc_chr_cmp(const void *a, const void *b) {
  return memcmp(a, b, 1);
}

uint8_t *qc_ref_tiles_bytes; // temp global, do not free().
int qc_tiles_slices(const void *a, const void *b) {
  OfsLen aol = *(OfsLen *)a;
  OfsLen bol = *(OfsLen *)b;
  uint32_t min_len = aol.len < bol.len ? aol.len : bol.len;
  int mc = memcmp(qc_ref_tiles_bytes + aol.ofs, qc_ref_tiles_bytes + bol.ofs, min_len);
  if (mc) return mc;
  if (min_len < bol.len) return -1;
  if (min_len < aol.len) return 1;
  return 0;
}

static inline bool eql_tiles_slices(OfsLen *a, OfsLen *b) {
  return a->len == b->len && !memcmp(qc_ref_tiles_bytes + a->ofs, qc_ref_tiles_bytes + b->ofs, a->len);
}

// append-only list of words.

typedef struct {
  VecOfsLen tiles_slices;
  VecByte tiles_bytes;
} Wordlist;

static inline Wordlist wordlist_new(void) {
  return (Wordlist){
      .tiles_slices = vecOfsLen_new(),
      .tiles_bytes = vecByte_new(),
    };
}

static inline void wordlist_free(Wordlist self[static 1]) {
  vecByte_free(&self->tiles_bytes);
  vecOfsLen_free(&self->tiles_slices);
}

static inline void wordlist_sort(Wordlist self[static 1]) {
  qc_ref_tiles_bytes = self->tiles_bytes.ptr;
  qsort(self->tiles_slices.ptr, self->tiles_slices.len, sizeof(OfsLen), qc_tiles_slices);
}

static inline void wordlist_dedup(Wordlist self[static 1]) {
  size_t r = 1;
  while (r < self->tiles_slices.len && !eql_tiles_slices(&self->tiles_slices.ptr[r], &self->tiles_slices.ptr[r - 1])) ++r;
  if (r < self->tiles_slices.len) {
    // [r] == [r-1]
    size_t w = r;
    while (++r < self->tiles_slices.len) {
      if (!eql_tiles_slices(&self->tiles_slices.ptr[r], &self->tiles_slices.ptr[w - 1])) {
        memcpy(&self->tiles_slices.ptr[w], &self->tiles_slices.ptr[r], sizeof(OfsLen));
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
} KwgcTransition;

#define VEC_ELT_NAME KwgcTransition
#define VEC_ELT_T KwgcTransition
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

typedef struct {
  VecKwgcTransition transitions; // Vec<kwgc_transition>
  VecU32 indexes; // Vec<uint32_t>
} KwgcTransitionStack;

static inline KwgcTransitionStack kwgc_transition_stack_new(void) {
  return (KwgcTransitionStack){
    .transitions = vecKwgcTransition_new(),
    .indexes = vecU32_new(),
  };
}

static inline void kwgc_transition_stack_free(KwgcTransitionStack self[static 1]) {
  vecU32_free(&self->indexes);
  vecKwgcTransition_free(&self->transitions);
}

static inline void kwgc_transition_stack_push(KwgcTransitionStack self[static 1], uint8_t tile) {
  vecKwgcTransition_push(&self->transitions, &(KwgcTransition){
      .arc_index = 0, // filled up later
      .tile = tile,
      .accepts = false,
    });
  uint32_t len = self->transitions.len;
  vecU32_push(&self->indexes, &len);
}

typedef struct {
  uint32_t arc_index; // refers to states.
  uint32_t next_index; // refers to states.
  uint8_t tile;
  bool accepts;
} KwgcState;

#define VEC_ELT_NAME KwgcState
#define VEC_ELT_T KwgcState
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

static inline void do_hash(uint64_t *hash, uint8_t data) {
  *hash = (*hash * 3467) ^ (data ^ 0xff);
}

static inline uint64_t kwgc_state_hash(KwgcState self[static 1]) {
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

static inline bool kwgc_state_eql(KwgcState a[static 1], KwgcState b[static 1]) {
#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
  return a->tile == b->tile &&
    a->accepts == b->accepts &&
    a->arc_index == b->arc_index &&
    a->next_index == b->next_index;
#ifndef __clang__
#pragma GCC diagnostic pop
#endif
}

#define KHM_K_NAME KwgcState
#define KHM_K_T KwgcState
#define KHM_K_HASHFUNC kwgc_state_hash
#define KHM_K_EQLFUNC kwgc_state_eql
#define KHM_V_NAME U32
#define KHM_V_T uint32_t
#include "generic_khm.c"
#undef KHM_V_T
#undef KHM_V_NAME
#undef KHM_K_EQLFUNC
#undef KHM_K_HASHFUNC
#undef KHM_K_T
#undef KHM_K_NAME

// for each i > 0, states[i].arc_index < i and states[i].next_index < i.
// this ensures states is already a topologically sorted DAG.
typedef struct {
  VecKwgcState states;
  KhmKwgcStateU32 states_finder;
} KwgcStateMaker;

static inline KwgcStateMaker kwgc_state_maker_new(void) {
  return (KwgcStateMaker){
      .states = vecKwgcState_new(),
      .states_finder = khmKwgcStateU32_new(),
    };
}

static inline void kwgc_state_maker_free(KwgcStateMaker self[static 1]) {
  khmKwgcStateU32_free(&self->states_finder);
  vecKwgcState_free(&self->states);
}

static inline uint32_t kwgc_state_maker_make_state(KwgcStateMaker self[static 1], VecKwgcTransition node_transitions[static 1], size_t target_len) {
  uint32_t ret = 0;
  for (size_t i = node_transitions->len; i-- > target_len; ) {
    KwgcTransition *node_transition = &node_transitions->ptr[i];
    KwgcState state = {
        .arc_index = node_transition->arc_index,
        .next_index = ret,
        .tile = node_transition->tile,
        .accepts = node_transition->accepts,
      };
    uint32_t *existing_state_index = khmKwgcStateU32_get(&self->states_finder, &state);
    if (existing_state_index) {
      ret = *existing_state_index;
    } else {
      ret = (uint32_t)self->states.len;
      vecKwgcState_push(&self->states, &state);
      khmKwgcStateU32_set(&self->states_finder, &state, &ret);
    }
  }
  return ret;
}

static inline void kwgc_transition_stack_pop(KwgcTransitionStack self[static 1], KwgcStateMaker state_maker[static 1]) {
  size_t start_of_batch = (size_t)self->indexes.ptr[--self->indexes.len];
  uint32_t new_arc_index = kwgc_state_maker_make_state(state_maker, &self->transitions, start_of_batch);
  self->transitions.ptr[start_of_batch - 1].arc_index = new_arc_index;
  self->transitions.len = start_of_batch;
}

static inline uint32_t kwgc_state_maker_make_dawg(KwgcStateMaker self[static 1], Wordlist sorted_machine_words[static 1], uint32_t dawg_start_state, bool is_gaddag_phase) {
  KwgcTransitionStack transition_stack = kwgc_transition_stack_new();
  for (size_t machine_word_index = 0; machine_word_index < sorted_machine_words->tiles_slices.len; ++machine_word_index) {
    OfsLen *this_word = &sorted_machine_words->tiles_slices.ptr[machine_word_index];
    uint32_t prefix_len = 0;
    if (machine_word_index > 0) {
      OfsLen *prev_word = &sorted_machine_words->tiles_slices.ptr[machine_word_index - 1];
      uint32_t prev_word_len = transition_stack.indexes.len; // this can be one less than prev_word->len for gaddag.
      uint32_t min_word_len = prev_word_len < this_word->len ? prev_word_len : this_word->len;
      while (prefix_len < min_word_len &&
          sorted_machine_words->tiles_bytes.ptr[prev_word->ofs + prefix_len] ==
          sorted_machine_words->tiles_bytes.ptr[this_word->ofs + prefix_len])
        ++prefix_len;
      for (uint32_t i = prefix_len; i < prev_word_len; ++i) {
        kwgc_transition_stack_pop(&transition_stack, self);
      }
    }
    for (uint32_t i = prefix_len; i < this_word->len; ++i) {
      kwgc_transition_stack_push(&transition_stack, sorted_machine_words->tiles_bytes.ptr[this_word->ofs + i]);
    }
    if (is_gaddag_phase && !sorted_machine_words->tiles_bytes.ptr[this_word->ofs + this_word->len - 1]) {
      --transition_stack.indexes.len;
      // gaddag["AC@"] points to dawg["CA"]
      uint32_t p = dawg_start_state;
      for (uint32_t i = this_word->len - 1; i > 0; --i) {
        uint8_t sought_tile = sorted_machine_words->tiles_bytes.ptr[this_word->ofs + i - 1];
        while (true) {
          KwgcState *pstate = &self->states.ptr[p];
          if (pstate->tile == sought_tile) {
            p = pstate->arc_index;
            break;
          }
          p = pstate->next_index;
        }
      }
      transition_stack.transitions.ptr[transition_stack.transitions.len - 1].arc_index = p;
    } else {
      transition_stack.transitions.ptr[transition_stack.transitions.len - 1].accepts = true;
    }
  }
  while (transition_stack.indexes.len) kwgc_transition_stack_pop(&transition_stack, self);
  uint32_t ret = kwgc_state_maker_make_state(self, &transition_stack.transitions, 0);
  kwgc_transition_stack_free(&transition_stack);
  return ret;
}

typedef struct {
  KwgcState *states;
  uint32_t states_len;
  uint32_t *head_indexes;
  uint32_t *to_end_lens; // using uint8_t costs runtime.
  uint32_t *destination;
  uint32_t num_written;
} KwgcStatesDefragger;

void kwgc_states_defragger_defrag_legacy(KwgcStatesDefragger self[static 1], uint32_t p) {
  p = self->head_indexes[p];
  uint32_t *dp = self->destination + p;
  if (*dp) return;
  uint32_t num = self->to_end_lens[p];
  // temp value to break self-cycles.
  *dp = (uint32_t)~0;
  uint32_t write_p = p;
  while (true) {
    uint32_t a = self->states[p].arc_index;
    if (a) kwgc_states_defragger_defrag_legacy(self, a);
    p = self->states[p].next_index;
    if (!p) break;
  }
  uint32_t initial_num_written = self->num_written;
  *dp = 0;
  for (uint32_t ofs = 0; ofs < num; ++ofs) {
    // prefer earlier index, so dawg part does not point to gaddag part.
    dp = self->destination + write_p;
    if (*dp) break;
    *dp = initial_num_written + ofs;
    write_p = self->states[write_p].next_index;
  }
  // Always += num even if some nodes are necessarily duplicated due to sharing by different prev_nodes.
  self->num_written += num;
}

void kwgc_states_defragger_defrag_magpie(KwgcStatesDefragger self[static 1], uint32_t p) {
  uint32_t *dp = self->destination + p;
  if (*dp) return;
  *dp = self->num_written;
  // non-legacy mode reserves the space first.
  uint32_t num = self->to_end_lens[p];
  self->num_written += num;
  while (true) {
    uint32_t a = self->states[p].arc_index;
    if (a) kwgc_states_defragger_defrag_magpie(self, a);
    p = self->states[p].next_index;
    if (!p) break;
  }
}

void kwgc_states_defragger_defrag_magpie_merged(KwgcStatesDefragger self[static 1], uint32_t p) {
  p = self->head_indexes[p];
  uint32_t *dp = self->destination + p;
  if (*dp) return;
  uint32_t initial_num_written = self->num_written;
  // temp value to break self-cycles.
  *dp = (uint32_t)~0;
  // non-legacy mode reserves the space first.
  uint32_t num = self->to_end_lens[p];
  self->num_written += num;
  uint32_t write_p = p;
  while (true) {
    uint32_t a = self->states[p].arc_index;
    if (a) kwgc_states_defragger_defrag_magpie_merged(self, a);
    p = self->states[p].next_index;
    if (!p) break;
  }
  *dp = 0;
  for (uint32_t ofs = 0; ofs < num; ++ofs) {
    // prefer earlier index, so dawg part does not point to gaddag part.
    dp = self->destination + write_p;
    if (*dp) break;
    *dp = initial_num_written + ofs;
    write_p = self->states[write_p].next_index;
  }
  // non-legacy mode already reserves the space.
}

// Each block has 16 entries (hardcoded).
// 16 entries of u32 make 64 bytes, which is a common cache line size.
// 0 <= block_len[i] <= 16, from (i << 4) the first block_len[i] are occupied.
// If block_len[i] < 16, blocks_with_len[block_len[i]] stack includes i.
typedef struct {
  VecByte block_len;
  VecU32 blocks_with_len[16];
} KwgcStatesDefraggerExperimentalParams;

static inline void kwgc_states_defragger_defrag_cache_friendly(KwgcStatesDefragger self[static 1], KwgcStatesDefraggerExperimentalParams params[static 1], uint32_t p) {
  p = self->head_indexes[p];
  uint32_t *dp = self->destination + p;
  if (*dp) return;
  // temp value to break self-cycles.
  *dp = (uint32_t)~0;
  // non-legacy mode reserves the space first.
  uint32_t num = self->to_end_lens[p];
  // choose a cache-friendly page to place these.
  uint32_t num_blocks = params->block_len.len;
  uint32_t initial_num_written = 0;
  if (num > 16) {
    uint8_t tmp_u8 = 0;
    // always even-align for 128 byte cache line machines.
    if ((num_blocks & 1) == 1) {
      vecU32_push(&params->blocks_with_len[0], &num_blocks);
      vecByte_push(&params->block_len, &tmp_u8);
      ++num_blocks;
    }
    initial_num_written = num_blocks << 4;
    uint32_t inner_num = num;
    tmp_u8 = 16;
    while (inner_num > 16) {
      vecByte_push(&params->block_len, &tmp_u8);
      inner_num -= 16;
    }
    // this can be between 1 to 16.
    if (inner_num < 16) {
      uint32_t tmp_u32 = params->block_len.len;
      vecU32_push(&params->blocks_with_len[inner_num], &tmp_u32);
    }
    tmp_u8 = inner_num;
    vecByte_push(&params->block_len, &tmp_u8);
  } else {
    // 1 <= num <= 16
    for (uint8_t required_gap = 16 - (uint8_t)num; ; --required_gap) { // 0 <= required_gap <= 15
      // if found, use it
      if (params->blocks_with_len[required_gap].len) {
        uint32_t place = params->blocks_with_len[required_gap].ptr[--params->blocks_with_len[required_gap].len];
        // use | instead of + because it cannot overflow
        initial_num_written = (place << 4) | required_gap;
        // repurpose this variable.
        required_gap += num; // 1 <= required_gap <= 16
        if (required_gap < 16) vecU32_push(&params->blocks_with_len[required_gap], &place);
        params->block_len.ptr[place] = required_gap;
        break;
      }
      // if 0, add new row.
      if (!required_gap) {
        initial_num_written = num_blocks << 4;
        if (num < 16) vecU32_push(&params->blocks_with_len[num], &num_blocks);
        uint8_t tmp_u8 = num;
        vecByte_push(&params->block_len, &tmp_u8);
        break;
      }
    }
  }
  uint32_t write_p = p;
  while (true) {
    uint32_t a = self->states[p].arc_index;
    if (a) kwgc_states_defragger_defrag_cache_friendly(self, params, a);
    p = self->states[p].next_index;
    if (!p) break;
  }
  *dp = 0;
  for (uint32_t ofs = 0; ofs < num; ++ofs) {
    // prefer earlier index, so dawg part does not point to gaddag part.
    dp = self->destination + write_p;
    if (*dp) break;
    *dp = initial_num_written + ofs;
    write_p = self->states[write_p].next_index;
  }
  // non-legacy mode already reserves the space.
}

uint32_t *qc_ref_num_ways; // temp global, do not free().
uint32_t *qc_ref_to_end_lens; // temp global, do not free().
int qc_build_experimental(const void *a, const void *b) {
  uint32_t pa = *(uint32_t *)a;
  uint32_t pb = *(uint32_t *)b;
  uint32_t num_ways_a = qc_ref_num_ways[pa];
  uint32_t num_ways_b = qc_ref_num_ways[pb];
  if (num_ways_b < num_ways_a) return -1;
  if (num_ways_b > num_ways_a) return 1;
  uint32_t to_end_lens_a = qc_ref_to_end_lens[pa];
  uint32_t to_end_lens_b = qc_ref_to_end_lens[pb];
  if (to_end_lens_b < to_end_lens_a) return -1;
  if (to_end_lens_b > to_end_lens_a) return 1;
  if (pa < pb) return -1;
  if (pa > pb) return 1;
  return 0;
}

void kwgc_states_defragger_build_experimental(KwgcStatesDefragger self[static 1], uint32_t num_ways[static 1], uint32_t top_indexes[static 1]) {
  uint32_t states_len_minus_one = self->states_len - 1;
  uint32_t *idxs = malloc_or_die(states_len_minus_one * sizeof(uint32_t));
  for (uint32_t p = 0; p < states_len_minus_one; ++p) idxs[p] = p + 1;
  qc_ref_num_ways = num_ways;
  qc_ref_to_end_lens = self->to_end_lens;
  qsort(idxs, states_len_minus_one, sizeof(uint32_t), qc_build_experimental);

  KwgcStatesDefraggerExperimentalParams params = {
      .block_len = vecByte_new(),
      .blocks_with_len = {
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
      },
    };
  {
    // num_written is either 1 or 2, both are < 16.
    uint8_t num_written_as_byte = (uint8_t)self->num_written;
    vecByte_push(&params.block_len, &num_written_as_byte);
  }
  {
    uint32_t zero = 0;
    vecU32_push(&params.blocks_with_len[self->num_written], &zero);
  }
  for (uint32_t i = 0; i < states_len_minus_one; ++i) {
    kwgc_states_defragger_defrag_cache_friendly(self, &params, top_indexes[idxs[i]]);
  }
  self->num_written = ((params.block_len.len - 1) << 4) + params.block_len.ptr[params.block_len.len - 1];

  for (uint32_t i = 16; i-- > 0; ) vecU32_free(&params.blocks_with_len[i]);
  vecByte_free(&params.block_len);
  free(idxs);
}

bool *qc_ref_used_in_dawg; // temp global, do not free().
int qc_build_wolges(const void *a, const void *b) {
  uint32_t pa = *(uint32_t *)a;
  uint32_t pb = *(uint32_t *)b;
  bool used_in_dawg_a = qc_ref_used_in_dawg[pa];
  bool used_in_dawg_b = qc_ref_used_in_dawg[pb];
  if (used_in_dawg_b < used_in_dawg_a) return -1;
  if (used_in_dawg_b > used_in_dawg_a) return 1;
  uint32_t num_ways_a = qc_ref_num_ways[pa];
  uint32_t num_ways_b = qc_ref_num_ways[pb];
  if (num_ways_b < num_ways_a) return -1;
  if (num_ways_b > num_ways_a) return 1;
  uint32_t to_end_lens_a = qc_ref_to_end_lens[pa];
  uint32_t to_end_lens_b = qc_ref_to_end_lens[pb];
  if (to_end_lens_b < to_end_lens_a) return -1;
  if (to_end_lens_b > to_end_lens_a) return 1;
  if (pa < pb) return -1;
  if (pa > pb) return 1;
  return 0;
}

void kwgc_states_defragger_build_wolges(KwgcStatesDefragger self[static 1], uint32_t num_ways[static 1], bool is_gaddag, uint32_t dawg_start_state) {
  uint32_t states_len_minus_one = self->states_len - 1;
  uint32_t *idxs = malloc_or_die(states_len_minus_one * sizeof(uint32_t));
  for (uint32_t p = 0; p < states_len_minus_one; ++p) idxs[p] = p + 1;
  qc_ref_num_ways = num_ways;
  qc_ref_to_end_lens = self->to_end_lens;
  if (is_gaddag) {
    // Check which nodes are used in dawg.
    bool *used_in_dawg = malloc_or_die(self->states_len * sizeof(bool));
    used_in_dawg[0] = false;
    uint32_t p = 1;
    for (; p <= dawg_start_state; ++p) used_in_dawg[p] = true;
    for (; p < self->states_len; ++p) used_in_dawg[p] = used_in_dawg[self->states[p].next_index];
    qc_ref_used_in_dawg = used_in_dawg;
    qsort(idxs, states_len_minus_one, sizeof(uint32_t), qc_build_wolges);
    free(used_in_dawg);
  } else {
    // All nodes are dawg nodes.
    qsort(idxs, states_len_minus_one, sizeof(uint32_t), qc_build_experimental);
  }

  KwgcStatesDefraggerExperimentalParams params = {
      .block_len = vecByte_new(),
      .blocks_with_len = {
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
        vecU32_new(), vecU32_new(), vecU32_new(), vecU32_new(),
      },
    };
  {
    // num_written is either 1 or 2, both are < 16.
    uint8_t num_written_as_byte = (uint8_t)self->num_written;
    vecByte_push(&params.block_len, &num_written_as_byte);
  }
  {
    uint32_t zero = 0;
    vecU32_push(&params.blocks_with_len[self->num_written], &zero);
  }
  for (uint32_t i = 0; i < states_len_minus_one; ++i) {
    kwgc_states_defragger_defrag_cache_friendly(self, &params, idxs[i]);
  }
  self->num_written = ((params.block_len.len - 1) << 4) + params.block_len.ptr[params.block_len.len - 1];

  for (uint32_t i = 16; i-- > 0; ) vecU32_free(&params.blocks_with_len[i]);
  vecByte_free(&params.block_len);
  free(idxs);
}

typedef void (*WriteNodeFn)(uint8_t *, uint32_t, bool, bool, uint8_t);

static inline void kwgc_write_node(uint8_t *pout, uint32_t defragged_arc_index, bool is_end, bool accepts, uint8_t tile) {
  pout[0] = defragged_arc_index;
  pout[1] = defragged_arc_index >> 8;
  pout[2] = ((defragged_arc_index >> 16) & 0x3f) | (uint8_t)(is_end << 6) | (uint8_t)(accepts << 7);
  pout[3] = tile;
}

// ret must initially be empty.
static void xwgc_build(VecU32 *ret, Wordlist sorted_machine_words[static 1], bool is_gaddag, BuildLayout build_layout, WriteNodeFn write_node, uint32_t max_nodes) {
  KwgcStateMaker state_maker = kwgc_state_maker_new();
  // The sink state always exists.
  vecKwgcState_push(&state_maker.states, &(KwgcState){
      .arc_index = 0,
      .next_index = 0,
      .tile = 0,
      .accepts = false,
    });
  uint32_t gaddag_start_state = 0;
  khmKwgcStateU32_set(&state_maker.states_finder, state_maker.states.ptr, &gaddag_start_state);
  uint32_t dawg_start_state = kwgc_state_maker_make_dawg(&state_maker, sorted_machine_words, 0, false);
  if (is_gaddag) {
    OfsLen cur_ofs_len = { .ofs = 0, .len = 0 };
    Wordlist gaddag_wl = wordlist_new();
    for (size_t machine_word_index = 0; machine_word_index < sorted_machine_words->tiles_slices.len; ++machine_word_index) {
      OfsLen *this_word = &sorted_machine_words->tiles_slices.ptr[machine_word_index];
      uint32_t prefix_len = 0;
      if (machine_word_index > 0) {
        OfsLen *prev_word = &sorted_machine_words->tiles_slices.ptr[machine_word_index - 1];
        uint32_t max_prefix_len = prev_word->len - 1; // - 1 because CAR -> CARE means we still need to emit RAC@.
        if (this_word->len < max_prefix_len) max_prefix_len = this_word->len;
        while (prefix_len < max_prefix_len &&
            sorted_machine_words->tiles_bytes.ptr[prev_word->ofs + prefix_len] ==
            sorted_machine_words->tiles_bytes.ptr[this_word->ofs + prefix_len])
          ++prefix_len;
      }
      // CARE = ERAC, RAC@, AC@, C@
      for (size_t i = this_word->len; i > 0; --i) {
        vecByte_push(&gaddag_wl.tiles_bytes, &sorted_machine_words->tiles_bytes.ptr[this_word->ofs + i - 1]);
      }
      cur_ofs_len.len = this_word->len;
      vecOfsLen_push(&gaddag_wl.tiles_slices, &cur_ofs_len);
      cur_ofs_len.ofs += cur_ofs_len.len;
      uint8_t zero = 0;
      for (size_t j = this_word->len - 1; j > prefix_len; --j) {
        for (size_t i = j; i > 0; --i) {
          vecByte_push(&gaddag_wl.tiles_bytes, &sorted_machine_words->tiles_bytes.ptr[this_word->ofs + i - 1]);
        }
        vecByte_push(&gaddag_wl.tiles_bytes, &zero);
        cur_ofs_len.len = j + 1;
        vecOfsLen_push(&gaddag_wl.tiles_slices, &cur_ofs_len);
        cur_ofs_len.ofs += cur_ofs_len.len;
      }
    }
    wordlist_sort(&gaddag_wl);
    gaddag_start_state = kwgc_state_maker_make_dawg(&state_maker, &gaddag_wl, dawg_start_state, true);
    wordlist_free(&gaddag_wl);
  }
  uint32_t *head_indexes = NULL;
  switch (build_layout) {
    case BuildLayout_Magpie:
      break;
    case BuildLayout_Legacy:
    case BuildLayout_MagpieMerged:
    case BuildLayout_Experimental:
    case BuildLayout_Wolges:
      head_indexes = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
      for (uint32_t p = 0; p < state_maker.states.len; ++p) head_indexes[p] = p;
      // point to immediate prev.
      for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
        head_indexes[state_maker.states.ptr[p].next_index] = p;
      }
      // head_indexes[0] is garbage, does not matter.
      // adjust to point to prev heads instead.
      for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
        head_indexes[p] = head_indexes[head_indexes[p]];
      }
  }
  uint32_t *to_end_lens = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  for (uint32_t p = 0; p < state_maker.states.len; ++p) {
    to_end_lens[p] = 1;
    uint32_t next = state_maker.states.ptr[p].next_index;
    if (next) to_end_lens[p] += to_end_lens[next];
  }
  uint32_t *destination = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  memset(destination, 0, state_maker.states.len * sizeof(uint32_t));
  uint32_t *num_ways = NULL;
  switch (build_layout) {
    case BuildLayout_Experimental:
    case BuildLayout_Wolges:
      num_ways = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
      memset(num_ways, 0, state_maker.states.len * sizeof(uint32_t));
      num_ways[dawg_start_state] = 1;
      if (is_gaddag) num_ways[gaddag_start_state] = 1;
      for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
        uint32_t this_num_ways = num_ways[p];
        // saturating add using cmov.
        uint32_t *pp_dest = num_ways + state_maker.states.ptr[p].next_index;
        if ((*pp_dest += this_num_ways) < this_num_ways) *pp_dest = (uint32_t)~0;
        pp_dest = num_ways + state_maker.states.ptr[p].arc_index;
        if ((*pp_dest += this_num_ways) < this_num_ways) *pp_dest = (uint32_t)~0;
      }
      break;
    case BuildLayout_Legacy:
    case BuildLayout_Magpie:
    case BuildLayout_MagpieMerged:
      break;
  }
  uint32_t *top_indexes = NULL;
  switch (build_layout) {
    case BuildLayout_Experimental:
      top_indexes = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
      memset(top_indexes, 0, state_maker.states.len * sizeof(uint32_t));
      for (uint32_t p = 1; p < state_maker.states.len; ++p) {
        uint32_t *pp_dest = top_indexes + state_maker.states.ptr[p].arc_index;
        *pp_dest = p | (uint32_t)-!!*pp_dest;
      }
      // [p] = 0 (no parent), parent_index, or !0 if > 1 parents.
      // if not unique, set [p] = p.
      for (uint32_t p = 0; p < state_maker.states.len; ++p) {
        uint32_t *pp_dest = top_indexes + p;
        if (*pp_dest == 0 || *pp_dest == (uint32_t)~0) *pp_dest = p;
      }
      // adjust to point to prev tops's heads instead.
      for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
        top_indexes[p] = head_indexes[top_indexes[top_indexes[p]]];
      }
      break;
    case BuildLayout_Legacy:
    case BuildLayout_Magpie:
    case BuildLayout_MagpieMerged:
    case BuildLayout_Wolges:
      break;
  }
  KwgcStatesDefragger states_defragger = {
      .states = (KwgcState *)state_maker.states.ptr,
      .states_len = state_maker.states.len,
      .head_indexes = head_indexes,
      .to_end_lens = to_end_lens,
      .destination = destination,
      .num_written = is_gaddag ? 2 : 1,
    };
  destination[0] = (uint32_t)~0; // useful for empty lexicon.
  switch (build_layout) {
    case BuildLayout_Legacy:
      kwgc_states_defragger_defrag_legacy(&states_defragger, dawg_start_state);
      if (is_gaddag) kwgc_states_defragger_defrag_legacy(&states_defragger, gaddag_start_state);
      break;
    case BuildLayout_Magpie:
      kwgc_states_defragger_defrag_magpie(&states_defragger, dawg_start_state);
      if (is_gaddag) kwgc_states_defragger_defrag_magpie(&states_defragger, gaddag_start_state);
      break;
    case BuildLayout_MagpieMerged:
      kwgc_states_defragger_defrag_magpie_merged(&states_defragger, dawg_start_state);
      if (is_gaddag) kwgc_states_defragger_defrag_magpie_merged(&states_defragger, gaddag_start_state);
      break;
    case BuildLayout_Experimental:
      kwgc_states_defragger_build_experimental(&states_defragger, num_ways, top_indexes);
      break;
    case BuildLayout_Wolges:
      kwgc_states_defragger_build_wolges(&states_defragger, num_ways, is_gaddag, dawg_start_state);
      break;
  }
  destination[0] = 0; // useful for empty lexicon.
  if (states_defragger.num_written > max_nodes) {
    fprintf(stderr, "this format cannot have %u nodes\n", states_defragger.num_written);
    free(destination);
    free(to_end_lens);
    free(head_indexes);
    kwgc_state_maker_free(&state_maker);
    return;
  }
  vecU32_ensure_cap_exact(ret, ret->len = states_defragger.num_written);
  memset(ret->ptr, 0, ret->len * sizeof(uint32_t)); // initialize gaps to 0 for determinism.
  write_node((uint8_t *)ret->ptr, destination[dawg_start_state], true, false, 0);
  if (is_gaddag) write_node((uint8_t *)(ret->ptr + 1), destination[gaddag_start_state], true, false, 0);
  for (uint32_t outer_p = 1; outer_p < state_maker.states.len; ++outer_p) {
    uint32_t dp = destination[outer_p];
    if (dp) {
      for (uint32_t p = outer_p; ; ++dp) {
        KwgcState *state = states_defragger.states + p;
        write_node((uint8_t *)(ret->ptr + dp), destination[state->arc_index], !state->next_index, state->accepts, state->tile);
        if (!state->next_index) break;
        p = state->next_index;
      }
    }
  }
  free(top_indexes);
  free(num_ways);
  free(destination);
  free(to_end_lens);
  free(head_indexes);
  kwgc_state_maker_free(&state_maker);
}

static inline void kbwgc_write_node(uint8_t *pout, uint32_t defragged_arc_index, bool is_end, bool accepts, uint8_t tile) {
  pout[0] = (tile & 0x3f) | (uint8_t)(is_end << 6) | (uint8_t)(accepts << 7);
  pout[1] = defragged_arc_index;
  pout[2] = defragged_arc_index >> 8;
  pout[3] = defragged_arc_index >> 16;
}

// commands

// the whole of a file, or of stdin for "-", with room for extra more bytes
uint8_t *read_all(const char *path, size_t len[static 1], size_t extra) {
  bool is_stdin = !strcmp(path, "-");
  FILE *f = is_stdin ? stdin : fopen(path, "rb");
  if (!f) { perror(path); return NULL; }
  size_t cap = (size_t)1 << 16, n = 0;
  uint8_t *buf = malloc_or_die(cap + extra);
  while (true) {
    n += fread(buf + n, 1, cap - n, f);
    if (n < cap) break;
    cap *= 2;
    buf = realloc_or_die(buf, cap + extra);
  }
  bool bad = ferror(f);
  if (!is_stdin && fclose(f)) bad = true;
  if (bad) { perror(path); free(buf); return NULL; }
  *len = n;
  return buf;
}

// to a file, or to stdout for "-"
bool write_all(const char *path, const void *ptr, size_t len) {
  bool is_stdout = !strcmp(path, "-");
  FILE *f = is_stdout ? stdout : fopen(path, "wb");
  if (!f) { perror(path); return false; }
  bool ok = fwrite(ptr, 1, len, f) == len;
  if (is_stdout) ok = !fflush(f) && ok;
  else if (fclose(f)) ok = false;
  if (!ok) perror(path);
  return ok;
}

bool do_kwg(const char *in, const char *out, ParsedTile tileset_parse(uint8_t *), BuildLayout build_layout, bool alphagrams, bool is_gaddag, bool is_kbwg) {
  bool errored = false;
  bool defer_free_file_content = false;
  bool defer_free_wl = false;
  bool defer_free_ret = false;
  size_t file_size;
  uint8_t *file_content = read_all(in, &file_size, 1); if (!file_content) goto errored; defer_free_file_content = true;
  file_content[file_size++] = '\n'; // sentinel
  OfsLen cur_ofs_len = { .ofs = 0, .len = 0 };
  Wordlist wl = wordlist_new(); defer_free_wl = true;
  for (size_t i = 0; i < file_size; ) {
    ParsedTile parsed_tile = tileset_parse(file_content + i);
    if (parsed_tile.len && parsed_tile.index > 0) { // ignore blank
      vecByte_push(&wl.tiles_bytes, &parsed_tile.index);
      i += parsed_tile.len;
      ++cur_ofs_len.len;
    } else if (file_content[i] <= ' ') {
      while (file_content[i] != '\n') ++i;
      ++i; // skip the newline
      if (cur_ofs_len.len > 0) {
        if (alphagrams) qsort(wl.tiles_bytes.ptr + cur_ofs_len.ofs, cur_ofs_len.len, sizeof(uint8_t), qc_chr_cmp);
        vecOfsLen_push(&wl.tiles_slices, &cur_ofs_len);
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
  VecU32 ret = vecU32_new(); defer_free_ret = true;
  xwgc_build(&ret, &wl, is_gaddag, build_layout, is_kbwg ? kbwgc_write_node : kwgc_write_node, is_kbwg ? 0x1000000 : 0x400000);
  if (!ret.len) goto errored;
  if (!write_all(out, ret.ptr, ret.len * sizeof(uint32_t))) goto errored;
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_ret) vecU32_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  return !errored;
}

// a leave's value as wolges's buildlex takes it: an optional sign, digits with
// at most one point, an optional exponent, and nothing else (no hex, nan, inf
// or space); then v * 1000, a float, from -2^31 up to but not 2^31, which a
// load rounds into an i32.
bool parse_leave_value(const char *s, float *val) {
  const char *p = s;
  bool digits = false;
  if (*p == '+' || *p == '-') ++p;
  while (*p >= '0' && *p <= '9') { ++p; digits = true; }
  if (*p == '.') for (++p; *p >= '0' && *p <= '9'; ++p) digits = true;
  if (!digits) return false;
  if (*p == 'e' || *p == 'E') {
    ++p;
    if (*p == '+' || *p == '-') ++p;
    if (!(*p >= '0' && *p <= '9')) return false;
    while (*p >= '0' && *p <= '9') ++p;
  }
  if (*p) return false;
  char *end;
  float v = strtof(s, &end);
  if (end != p) return false;
  float milli = v * 1000.0f;
  if (!(milli >= -2147483648.0f && milli < 2147483648.0f)) return false;
  *val = v;
  return true;
}

bool do_klv(const char *in, const char *out_path, ParsedTile tileset_parse(uint8_t *), BuildLayout build_layout) {
  bool errored = false;
  bool defer_free_file_content = false;
  bool defer_free_wl = false;
  bool defer_free_ret = false;
  bool defer_free_out = false;
  size_t file_size;
  uint8_t *file_content = read_all(in, &file_size, 1); if (!file_content) goto errored; defer_free_file_content = true;
  file_content[file_size++] = '\n'; // sentinel
  OfsLen cur_ofs_len = { .ofs = 0, .len = 0 };
  Wordlist wl = wordlist_new(); defer_free_wl = true;
  bool this_is_big_endian = is_big_endian();
  for (size_t i = 0; i < file_size; ) {
    ParsedTile parsed_tile = tileset_parse(file_content + i);
    if (parsed_tile.len) { // allow blank
      vecByte_push(&wl.tiles_bytes, &parsed_tile.index);
      i += parsed_tile.len;
      ++cur_ofs_len.len;
    } else if (file_content[i] == ',') {
      size_t orig_i = i;
      while (file_content[i] != '\n') ++i;
      file_content[i] = '\0'; // end the value at its line
      if (file_content[i - 1] == '\r') file_content[i - 1] = '\0'; // a \r\n line
      float val;
      if (!parse_leave_value((char *)(file_content + orig_i + 1), &val)) {
        fprintf(stderr, "bad value at offset %zu\n", orig_i + 1);
        goto errored;
      }
      ++i; // skip the newline
      if (cur_ofs_len.len > 0) {
        qsort(wl.tiles_bytes.ptr + cur_ofs_len.ofs, cur_ofs_len.len, sizeof(uint8_t), qc_chr_cmp);
        if (this_is_big_endian) {
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 3);
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 2);
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 1);
          vecByte_push(&wl.tiles_bytes, (uint8_t *)&val);
        } else {
          vecByte_push(&wl.tiles_bytes, (uint8_t *)&val);
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 1);
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 2);
          vecByte_push(&wl.tiles_bytes, ((uint8_t *)&val) + 3);
        }
        vecOfsLen_push(&wl.tiles_slices, &cur_ofs_len);
        cur_ofs_len.ofs += cur_ofs_len.len + 4; // sizeof(float)
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
  VecU32 ret = vecU32_new(); defer_free_ret = true;
  xwgc_build(&ret, &wl, false, build_layout, kwgc_write_node, 0x400000);
  if (!ret.len) goto errored;
  size_t out_len = ret.len + wl.tiles_slices.len + 2;
  uint8_t *out = malloc_or_die(out_len * sizeof(uint32_t)); defer_free_out = true;
  uint8_t *pout = out;
  *pout++ = ret.len;
  *pout++ = ret.len >> 8;
  *pout++ = ret.len >> 16;
  *pout++ = ret.len >> 24;
  memcpy(pout, ret.ptr, ret.len * sizeof(uint32_t));
  pout += ret.len * sizeof(uint32_t);
  *pout++ = wl.tiles_slices.len;
  *pout++ = wl.tiles_slices.len >> 8;
  *pout++ = wl.tiles_slices.len >> 16;
  *pout++ = wl.tiles_slices.len >> 24;
  for (size_t i = 0; i < wl.tiles_slices.len; ++i) {
    OfsLen *this_word = &wl.tiles_slices.ptr[i];
    uint8_t *p = &wl.tiles_bytes.ptr[this_word->ofs + this_word->len];
    *pout++ = *p++;
    *pout++ = *p++;
    *pout++ = *p++;
    *pout++ = *p++;
  }
  if (!write_all(out_path, out, out_len * sizeof(uint32_t))) goto errored;
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_out) free(out);
  if (defer_free_ret) vecU32_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  return !errored;
}

typedef struct { uint32_t p : 22; bool e : 1, d : 1; uint8_t c : 8; } KwgNode; // compiler-specific UB.

#define XWG_NODE_T KwgNode
#define XWG_NAME kwg
#include "generic_xwg_read.c"
#undef XWG_NAME
#undef XWG_NODE_T

typedef struct { uint8_t c : 6; bool e : 1, d : 1; uint32_t p : 24; } KbwgNode; // compiler-specific UB.

#define XWG_NODE_T KbwgNode
#define XWG_NAME kbwg
#include "generic_xwg_read.c"
#undef XWG_NAME
#undef XWG_NODE_T

void dump_klv2(KwgNode *kwg, VecChar *word, uint32_t p, Tile tileset[static 1], float **klv_ptr) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t label_len = strlen(tileset[kwg[p].c].label);
    size_t len = orig_len + label_len;
    vecChar_ensure_cap(word, len);
    memcpy(word->ptr + orig_len, tileset[kwg[p].c].label, label_len);
    word->len = len;
    if (kwg[p].d) {
      // find the required precision between 1 and 16.
      char s[128];
      float check;
#define precision_le(n, t, f) (((snprintf(s, sizeof(s), "%." #n "f", **klv_ptr), sscanf(s, "%f", &check)) == 1 && check == **klv_ptr) ? t : f)
      int precision = precision_le(8,
        precision_le(4,
          precision_le(2, precision_le(1, 1, 2), precision_le(3, 3, 4)),
          precision_le(6, precision_le(5, 5, 6), precision_le(7, 7, 8))),
        precision_le(12,
          precision_le(10, precision_le(9, 9, 10), precision_le(11, 11, 12)),
          precision_le(14, precision_le(13, 13, 14), precision_le(15, 15, 16))));
      printf("%.*s,%.*f\n", (int)len, word->ptr, precision, *(*klv_ptr)++);
    }
    if (kwg[p].p) dump_klv2(kwg, word, kwg[p].p, tileset, klv_ptr);
    if (kwg[p].e) break;
  }
  word->len = orig_len;
}

bool do_read_klv(const char *in, Tile tileset[static 1]) {
  bool errored = false;
  bool defer_free_file_content = false;
  bool defer_free_word = false;
  size_t file_size;
  uint8_t *file_content = read_all(in, &file_size, 0); if (!file_content) goto errored; defer_free_file_content = true;
  if (is_big_endian()) swap_bytes_32(file_content, file_size);
  uint32_t num_kwg_nodes = *(uint32_t *)file_content;
  KwgNode *kwg = (KwgNode *)(file_content + sizeof(uint32_t));
  float *klv_values = (float *)(file_content + sizeof(uint32_t) * (2 + num_kwg_nodes));
  VecChar word = vecChar_new(); defer_free_word = true;
  dump_klv2(kwg, &word, kwg[0].p, tileset, &klv_values);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  return !errored;
}

// --tiles: an alphabet wolges bundles, by the name its buildlex takes, or an
// alphabet file read as buildlex reads one (alphabet.rs new_static_from_text):
// UTF-8 text, a tile a line -- label, blank label, count (0 to 255), score
// (-32 to 32), vowel (0 or 1), the number of alias labels and those, the
// number of alias blank labels and those; 1 to 64 tiles

bool tiles_by_name(const char *name, Tile **tileset, ParsedTile (**tileset_parse)(uint8_t *)) {
  const struct {
    const char *name;
    Tile *tileset;
    ParsedTile (*tileset_parse)(uint8_t *);
  } named[] = {
    { "catalan", catalan_tileset, catalan_tileset_parse },
    { "decimal", decimal_tileset, decimal_tileset_parse },
    { "dutch", dutch_tileset, dutch_tileset_parse },
    { "english", english_tileset, english_tileset_parse },
    { "french", french_tileset, french_tileset_parse },
    { "german", german_tileset, german_tileset_parse },
    { "hex", hex_tileset, hex_tileset_parse },
    { "hong-kong-english", hong_kong_english_tileset, hong_kong_english_tileset_parse },
    { "norwegian", norwegian_tileset, norwegian_tileset_parse },
    { "polish", polish_tileset, polish_tileset_parse },
    { "slovene", slovene_tileset, slovene_tileset_parse },
    { "spanish", spanish_tileset, spanish_tileset_parse },
    { "super-catalan", super_catalan_tileset, super_catalan_tileset_parse },
    { "super-english", super_english_tileset, super_english_tileset_parse },
    { "swedish", swedish_tileset, swedish_tileset_parse },
  };
  for (size_t i = 0; i < sizeof(named) / sizeof(named[0]); ++i) {
    if (!strcmp(name, named[i].name)) {
      *tileset = named[i].tileset;
      *tileset_parse = named[i].tileset_parse;
      return true;
    }
  }
  return false;
}

// as Rust's str::from_utf8: no stray continuation byte, no overlong form, no
// surrogate, nothing past U+10FFFF
static bool is_utf8(const uint8_t *s, size_t len) {
  for (size_t i = 0; i < len; ) {
    uint8_t c = s[i];
    size_t n;
    uint32_t min, cp;
    if (c < 0x80) {
      ++i;
      continue;
    } else if ((c & 0xe0) == 0xc0) {
      n = 1; min = 0x80; cp = (uint32_t)(c & 0x1f);
    } else if ((c & 0xf0) == 0xe0) {
      n = 2; min = 0x800; cp = (uint32_t)(c & 0x0f);
    } else if ((c & 0xf8) == 0xf0) {
      n = 3; min = 0x10000; cp = (uint32_t)(c & 0x07);
    } else {
      return false;
    }
    if (len - i <= n) return false;
    for (size_t k = 1; k <= n; ++k) {
      if ((s[i + k] & 0xc0) != 0x80) return false;
      cp = (cp << 6) | (uint32_t)(s[i + k] & 0x3f);
    }
    if (cp < min || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) return false;
    i += n + 1;
  }
  return true;
}

// the length of the whitespace character at p, as Rust's char::is_whitespace
// takes it (U+0009 to U+000D, U+0020, U+0085, U+00A0, U+1680, U+2000 to
// U+200A, U+2028, U+2029, U+202F, U+205F, U+3000), else 0; p is in UTF-8 text
static size_t ws_len(const uint8_t *p) {
  if ((p[0] >= 0x09 && p[0] <= 0x0d) || p[0] == 0x20) return 1;
  if (p[0] == 0xc2 && (p[1] == 0x85 || p[1] == 0xa0)) return 2;
  if (p[0] == 0xe1 && p[1] == 0x9a && p[2] == 0x80) return 3;
  if (p[0] == 0xe2 && p[1] == 0x80 && ((p[2] >= 0x80 && p[2] <= 0x8a) || p[2] == 0xa8 || p[2] == 0xa9 || p[2] == 0xaf)) return 3;
  if (p[0] == 0xe2 && p[1] == 0x81 && p[2] == 0x9f) return 3;
  if (p[0] == 0xe3 && p[1] == 0x80 && p[2] == 0x80) return 3;
  return 0;
}

// the next token of the line [*p, end), past whitespace
static bool next_token(const uint8_t **p, const uint8_t *end, const uint8_t **tok, size_t len[static 1]) {
  size_t w;
  while (*p < end && (w = ws_len(*p))) *p += w;
  if (*p == end) return false;
  *tok = *p;
  while (*p < end && !ws_len(*p)) ++*p;
  *len = (size_t)(*p - *tok);
  return true;
}

// a token as Rust's from_str reads an integer: an optional '+' (or '-' when
// signed) and decimal digits, nothing else; then within [lo, hi]
static bool token_int(const uint8_t *tok, size_t len, bool is_signed, long long lo, long long hi, long long v[static 1]) {
  size_t i = 0;
  bool neg = false;
  if (i < len && (tok[i] == '+' || (is_signed && tok[i] == '-'))) neg = tok[i++] == '-';
  if (i == len) return false;
  long long x = 0;
  for (; i < len; ++i) {
    if (tok[i] < '0' || tok[i] > '9') return false;
    if (x > (LLONG_MAX - 9) / 10) return false;
    x = x * 10 + (tok[i] - '0');
  }
  if (neg) x = -x;
  if (x < lo || x > hi) return false;
  *v = x;
  return true;
}

typedef struct {
  const uint8_t *bytes;
  size_t len;
  uint8_t index;
} FileLabel;

typedef struct {
  FileLabel *labels;
  size_t num_labels, cap_labels;
  size_t start[257]; // labels by first byte: [start[b], start[b + 1])
} FileReader;

// as buildlex's readers: a word takes every tile but 0 by its labels, its
// blank labels and their aliases; a leave also takes tile 0 by its label and
// that label's aliases (alphabet.rs new_for_words, new_for_racks)
FileReader file_words, file_racks; // global: a tileset's parse takes only the text

static void file_reader_push(FileReader *r, const char *label, size_t index) {
  if (r->num_labels == r->cap_labels) {
    r->cap_labels = r->cap_labels ? r->cap_labels * 2 : 128;
    r->labels = realloc_or_die(r->labels, r->cap_labels * sizeof(FileLabel));
  }
  r->labels[r->num_labels++] = (FileLabel){ .bytes = (const uint8_t *)label, .len = strlen(label), .index = (uint8_t)index };
}

// by first byte, then the longer first, then the lower tile (alphabet.rs
// new_for_tiles), so the first label that matches is the greedy match
static int qc_file_labels(const void *a, const void *b) {
  const FileLabel *la = a, *lb = b;
  if (la->bytes[0] != lb->bytes[0]) return la->bytes[0] < lb->bytes[0] ? -1 : 1;
  if (la->len != lb->len) return la->len > lb->len ? -1 : 1;
  return (la->index > lb->index) - (la->index < lb->index);
}

static void file_reader_index(FileReader *r) {
  qsort(r->labels, r->num_labels, sizeof(FileLabel), qc_file_labels);
  size_t i = 0;
  for (size_t b = 0; b < 256; ++b) {
    r->start[b] = i;
    while (i < r->num_labels && (size_t)r->labels[i].bytes[0] == b) ++i;
  }
  r->start[256] = i;
}

// a label holds no whitespace, so the text's line end stops every compare
static ParsedTile file_parse(const FileReader *r, const uint8_t *ptr) {
  for (size_t i = r->start[*ptr]; i < r->start[*ptr + 1]; ++i) {
    const FileLabel *l = &r->labels[i];
    size_t n = 0;
    while (n < l->len && ptr[n] == l->bytes[n]) ++n;
    if (n == l->len) return (ParsedTile){ .len = n, .index = l->index };
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

ParsedTile file_words_parse(uint8_t *ptr) { return file_parse(&file_words, ptr); }
ParsedTile file_racks_parse(uint8_t *ptr) { return file_parse(&file_racks, ptr); }

typedef enum { Label_Main, Label_Alias, Label_Blank, Label_BlankAlias } LabelKind;

typedef struct {
  char *s;
  size_t tile;
  LabelKind kind;
} FileEntry;

bool tiles_from_file(const char *path, Tile **tileset) {
  size_t len;
  uint8_t *text = read_all(path, &len, 0);
  if (!text) {
    fprintf(stderr, "\"%s\" is not bundled tiles or a readable file\n", path);
    return false;
  }
  if (!is_utf8(text, len)) {
    fprintf(stderr, "\"%s\" is not bundled tiles or a readable file: not UTF-8\n", path);
    return false;
  }
  FileEntry *entries = NULL;
  size_t num_entries = 0, cap_entries = 0, num_tiles = 0, line_no = 0;
  const char *why = NULL;
  const uint8_t *end_of_text = text + len;
  // lines as Rust's str::lines: split at '\n', and a final '\n' ends the last
  // line rather than starting an empty one; a '\r' is whitespace to a token
  for (const uint8_t *line = text; line < end_of_text; ) {
    const uint8_t *eol = memchr(line, '\n', (size_t)(end_of_text - line));
    if (!eol) eol = end_of_text;
    ++line_no;
    const uint8_t *p = line;
    // each token in turn: the labels, then the five numbers between them
    long long freq, score, vowel, count[2];
    for (int field = 0; field < 7 && !why; ++field) {
      const uint8_t *tok;
      size_t tok_len;
      if (!next_token(&p, eol, &tok, &tok_len)) { why = "not enough tokens"; break; }
      switch (field) {
      case 0: case 1:
        if (memchr(tok, '\0', tok_len)) { why = "a label holds a nul byte"; break; }
        if (num_entries == cap_entries) {
          cap_entries = cap_entries ? cap_entries * 2 : 256;
          entries = realloc_or_die(entries, cap_entries * sizeof(FileEntry));
        }
        entries[num_entries] = (FileEntry){ .s = malloc_or_die(tok_len + 1), .tile = num_tiles, .kind = field ? Label_Blank : Label_Main };
        memcpy(entries[num_entries].s, tok, tok_len);
        entries[num_entries++].s[tok_len] = '\0';
        break;
      case 2:
        if (!token_int(tok, tok_len, false, 0, 255, &freq)) why = "a count is a number from 0 to 255";
        break;
      case 3:
        if (!token_int(tok, tok_len, true, -128, 127, &score)) why = "a score is a number from -128 to 127";
        else if (score < -32 || score > 32) why = "a tile scores from -32 to 32";
        break;
      case 4:
        if (!token_int(tok, tok_len, false, 0, 255, &vowel)) why = "a vowel flag is a number from 0 to 255";
        else if (vowel > 1) why = "invalid bool";
        break;
      case 5: case 6:
        if (!token_int(tok, tok_len, false, 0, LLONG_MAX, &count[field - 5])) { why = "an alias count is a number"; break; }
        for (long long a = 0; a < count[field - 5]; ++a) {
          if (!next_token(&p, eol, &tok, &tok_len)) { why = "not enough tokens"; break; }
          if (memchr(tok, '\0', tok_len)) { why = "a label holds a nul byte"; break; }
          if (num_entries == cap_entries) {
            cap_entries = cap_entries ? cap_entries * 2 : 256;
            entries = realloc_or_die(entries, cap_entries * sizeof(FileEntry));
          }
          entries[num_entries] = (FileEntry){ .s = malloc_or_die(tok_len + 1), .tile = num_tiles, .kind = field == 5 ? Label_Alias : Label_BlankAlias };
          memcpy(entries[num_entries].s, tok, tok_len);
          entries[num_entries++].s[tok_len] = '\0';
        }
        break;
      }
    }
    if (why) break;
    const uint8_t *extra;
    size_t extra_len;
    if (next_token(&p, eol, &extra, &extra_len)) { why = "too many tokens"; break; }
    ++num_tiles;
    line = eol < end_of_text ? eol + 1 : eol;
  }
  if (why) {
    fprintf(stderr, "%s: line %zu: %s\n", path, line_no, why);
    return false;
  }
  if (num_tiles < 1 || num_tiles > 64) {
    fprintf(stderr, "%s: an alphabet has from 1 to 64 tiles\n", path);
    return false;
  }
  Tile *out = malloc_or_die(num_tiles * sizeof(Tile));
  for (size_t i = 0; i < num_entries; ++i) {
    FileEntry *e = &entries[i];
    if (e->kind == Label_Main) out[e->tile].label = e->s;
    if (e->kind == Label_Blank) out[e->tile].blank_label = e->s;
    if (e->tile > 0 || e->kind == Label_Main || e->kind == Label_Alias) file_reader_push(&file_racks, e->s, e->tile);
    if (e->tile > 0) file_reader_push(&file_words, e->s, e->tile);
  }
  file_reader_index(&file_words);
  file_reader_index(&file_racks);
  *tileset = out;
  return true;
}

// getopt_long from the start of another argv
static void reset_getopt(void) {
#ifdef __GLIBC__
  optind = 0;
#else
  optreset = 1;
  optind = 1;
#endif
}

static bool pick(const char *opt, const char *val, const char *const names[], int num_names, int index[static 1]) {
  for (int i = 0; i < num_names; ++i) {
    if (!strcmp(val, names[i])) {
      *index = i;
      return true;
    }
  }
  fprintf(stderr, "--%s cannot be \"%s\"\n", opt, val);
  return false;
}

static const char *const layout_names[] = { "wolges", "magpie", "magpiemerged", "legacy", "experimental" };
static const BuildLayout layouts[] = { BuildLayout_Wolges, BuildLayout_Magpie, BuildLayout_MagpieMerged, BuildLayout_Legacy, BuildLayout_Experimental };

static bool positionals(int argc, char **argv, int want, const char *what) {
  if (argc - optind == want) return true;
  fprintf(stderr, "%s takes %s\n", argv[0], what);
  return false;
}

bool run_kwg(int argc, char **argv, ParsedTile tileset_parse(uint8_t *)) {
  static const struct option opts[] = {
    { "graph", required_argument, NULL, 'g' },
    { "words", required_argument, NULL, 'w' },
    { "layout", required_argument, NULL, 'l' },
    { "kbwg", no_argument, NULL, 'b' },
    { NULL, 0, NULL, 0 },
  };
  int graph = -1, words = 0, layout = 0;
  bool kbwg = false;
  for (int c; (c = getopt_long(argc, argv, "", opts, NULL)) != -1; ) {
    switch (c) {
    case 'g': if (!pick("graph", optarg, (const char *const[]){ "gaddawg", "dawg" }, 2, &graph)) return false; break;
    case 'w': if (!pick("words", optarg, (const char *const[]){ "plain", "alphagrams" }, 2, &words)) return false; break;
    case 'l': if (!pick("layout", optarg, layout_names, 5, &layout)) return false; break;
    case 'b': kbwg = true; break;
    default: return false;
    }
  }
  if (!positionals(argc, argv, 2, "the words, one a line, and the word graph to write")) return false;
  bool alphagrams = words == 1;
  // gaddawg, or a dawg for alphagrams, unless --graph says
  bool is_gaddag = graph == -1 ? !alphagrams : graph == 0;
  if (!strcmp(argv[optind + 1], "-")) time_goes_to_stderr = true;
  return do_kwg(argv[optind], argv[optind + 1], tileset_parse, layouts[layout], alphagrams, is_gaddag, kbwg);
}

bool run_klv(int argc, char **argv, ParsedTile tileset_parse(uint8_t *)) {
  static const struct option opts[] = {
    { "klv", required_argument, NULL, 'k' },
    { "layout", required_argument, NULL, 'l' },
    { NULL, 0, NULL, 0 },
  };
  int klv = 0, layout = 0;
  for (int c; (c = getopt_long(argc, argv, "", opts, NULL)) != -1; ) {
    switch (c) {
    case 'k': if (!pick("klv", optarg, (const char *const[]){ "2" }, 1, &klv)) return false; break;
    case 'l': if (!pick("layout", optarg, layout_names, 5, &layout)) return false; break;
    default: return false;
    }
  }
  if (!positionals(argc, argv, 2, "the leaves and their values, leave,value a line, and the leave table to write")) return false;
  if (!strcmp(argv[optind + 1], "-")) time_goes_to_stderr = true;
  return do_klv(argv[optind], argv[optind + 1], tileset_parse, layouts[layout]);
}

// the reader's output: stdout, or the file stdout becomes
static bool output_to(const char *out) {
  time_goes_to_stderr = true;
  if (!strcmp(out, "-")) return true;
  if (freopen(out, "w", stdout)) return true;
  perror(out);
  return false;
}

bool run_read_kwg(int argc, char **argv, Tile tileset[static 1]) {
  static const struct option opts[] = {
    { "graph", required_argument, NULL, 'g' },
    { "kbwg", no_argument, NULL, 'b' },
    { NULL, 0, NULL, 0 },
  };
  int graph = 0;
  bool kbwg = false;
  for (int c; (c = getopt_long(argc, argv, "", opts, NULL)) != -1; ) {
    switch (c) {
    case 'g': if (!pick("graph", optarg, (const char *const[]){ "dawg", "gaddag" }, 2, &graph)) return false; break;
    case 'b': kbwg = true; break;
    default: return false;
    }
  }
  if (!positionals(argc, argv, 2, "the word graph and the words to write")) return false;
  if (!output_to(argv[optind + 1])) return false;
  return kbwg ? do_lang_rkbwg(argv[optind], tileset, graph) : do_lang_rkwg(argv[optind], tileset, graph);
}

bool run_read_klv(int argc, char **argv, Tile tileset[static 1]) {
  static const struct option opts[] = {
    { NULL, 0, NULL, 0 },
  };
  if (getopt_long(argc, argv, "", opts, NULL) != -1) return false;
  if (!positionals(argc, argv, 2, "the leave table and the leave,value lines to write")) return false;
  if (!output_to(argv[optind + 1])) return false;
  return do_read_klv(argv[optind], tileset);
}

void usage(FILE *f) {
  fputs(
    "usage: kwgc [--tiles NAME|FILE] <task> ...\n"
    "build word graphs and leave tables\n"
    "\n"
    "  --tiles NAME|FILE  bundled tiles or an alphabet file [default: english]:\n"
    "                     catalan, decimal, dutch, english, french, german, hex,\n"
    "                     hong-kong-english, norwegian, polish, slovene, spanish,\n"
    "                     super-catalan, super-english, swedish\n"
    "\n"
    "  kwg [--graph gaddawg|dawg] [--words plain|alphagrams] [--layout L] [--kbwg] WORDS OUT\n"
    "    generate a word graph from a word list [default: gaddawg; dawg for --words alphagrams]\n"
    "  klv [--klv 2] [--layout L] LEAVES OUT\n"
    "    generate a leave table from leave,value lines\n"
    "  read-kwg [--graph dawg|gaddag] [--kbwg] KWG OUT\n"
    "    read a word graph's words, on a little-endian system\n"
    "  read-klv KLV OUT\n"
    "    read a leave table's leave,value lines, on a little-endian system\n"
    "\n"
    "  --layout: wolges (default), magpie, magpiemerged, legacy, experimental\n"
    "  input/output files can be \"-\" (not advisable for binary files)\n",
    f);
}

int main(int argc, char **argv) {
  struct timeval tv_start = now();
  static const struct option opts[] = {
    { "tiles", required_argument, NULL, 't' },
    { "help", no_argument, NULL, 'h' },
    { NULL, 0, NULL, 0 },
  };
  const char *tiles = "english";
  for (int c; (c = getopt_long(argc, argv, "+h", opts, NULL)) != -1; ) {
    if (c == 't') {
      tiles = optarg;
    } else if (c == 'h') {
      usage(stdout);
      return 0;
    } else {
      usage(stderr);
      return 2;
    }
  }
  if (optind >= argc) {
    usage(stderr);
    return 2;
  }
  Tile *tileset;
  ParsedTile (*words_parse)(uint8_t *);
  ParsedTile (*racks_parse)(uint8_t *);
  if (tiles_by_name(tiles, &tileset, &words_parse)) {
    racks_parse = words_parse;
  } else {
    if (!tiles_from_file(tiles, &tileset)) return 1;
    words_parse = file_words_parse;
    racks_parse = file_racks_parse;
  }
  const char *task = argv[optind];
  int task_argc = argc - optind;
  char **task_argv = argv + optind;
  reset_getopt();
  bool ok;
  if (!strcmp(task, "kwg")) {
    ok = run_kwg(task_argc, task_argv, words_parse);
  } else if (!strcmp(task, "klv")) {
    ok = run_klv(task_argc, task_argv, racks_parse);
  } else if (!strcmp(task, "read-kwg")) {
    ok = run_read_kwg(task_argc, task_argv, tileset);
  } else if (!strcmp(task, "read-klv")) {
    ok = run_read_klv(task_argc, task_argv, tileset);
  } else {
    fprintf(stderr, "no task \"%s\"\n", task);
    usage(stderr);
    return 2;
  }
  if (!ok) return 1;
  struct timeval tv_end = now();
  FILE *time_stream = time_goes_to_stderr ? stderr : stdout;
  fputs("time taken: ", time_stream);
  fprint_dur_us(time_stream, tv_end, tv_start);
  fputs("s\n", time_stream);
  return 0;
}
