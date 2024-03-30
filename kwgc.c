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
  return a->tile == b->tile &&
    a->accepts == b->accepts &&
    a->arc_index == b->arc_index &&
    a->next_index == b->next_index;
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
  uint32_t *prev_indexes;
  uint32_t *destination;
  uint32_t num_written;
} KwgcStatesDefragger;

void kwgc_states_defragger_defrag(KwgcStatesDefragger self[static 1], uint32_t p) {
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

// ret must initially be empty.
void kwgc_build(VecU32 *ret, Wordlist sorted_machine_words[static 1], bool is_gaddag) {
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
  uint32_t *prev_indexes = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  memset(prev_indexes, 0, state_maker.states.len * sizeof(uint32_t));
  for (uint32_t p = state_maker.states.len - 1; p > 0; --p) {
    prev_indexes[state_maker.states.ptr[p].next_index] = p;
  }
  // prev_indexes[0] is garbage, does not matter.
  uint32_t *destination = malloc_or_die(state_maker.states.len * sizeof(uint32_t));
  memset(destination, 0, state_maker.states.len * sizeof(uint32_t));
  KwgcStatesDefragger states_defragger = {
      .states = (KwgcState *)state_maker.states.ptr,
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
  vecU32_ensure_cap_exact(ret, ret->len = states_defragger.num_written);
  kwgc_write_node((uint8_t *)ret->ptr, destination[dawg_start_state], true, false, 0);
  if (is_gaddag) kwgc_write_node((uint8_t *)(ret->ptr + 1), destination[gaddag_start_state], true, false, 0);
  for (uint32_t outer_p = 1; outer_p < state_maker.states.len; ++outer_p) {
    if (!prev_indexes[outer_p]) {
      uint32_t dp = destination[outer_p];
      if (dp) {
        for (uint32_t p = outer_p; ; ++dp) {
          KwgcState *state = states_defragger.states + p;
          kwgc_write_node((uint8_t *)(ret->ptr + dp), destination[state->arc_index], !state->next_index, state->accepts, state->tile);
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

bool do_lang_kwg(char **argv, ParsedTile tileset_parse(uint8_t *), int mode) {
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
        if (mode == 2) qsort(wl.tiles_bytes.ptr + cur_ofs_len.ofs, cur_ofs_len.len, sizeof(uint8_t), qc_chr_cmp);
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
  kwgc_build(&ret, &wl, mode == 1);
  if (!ret.len) goto errored;
  f = fopen(argv[3], "wb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fwrite(ret.ptr, sizeof(uint32_t), ret.len, f) != ret.len) { perror("fwrite"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_ret) vecU32_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

bool do_lang_klv2(char **argv, ParsedTile tileset_parse(uint8_t *)) {
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
      file_content[i] = '\0'; // prevent sscanf from crashing
      float val;
      if (sscanf((char *)(file_content + orig_i + 1), "%f", &val) != 1) {
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
  kwgc_build(&ret, &wl, false);
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
  f = fopen(argv[3], "wb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fwrite(out, sizeof(uint32_t), out_len, f) != out_len) { perror("fwrite"); goto errored; }
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_out) free(out);
  if (defer_free_ret) vecU32_free(&ret);
  if (defer_free_wl) wordlist_free(&wl);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

typedef struct { uint32_t p : 22; bool e : 1, d : 1; uint8_t c : 8; } KwgNode; // compiler-specific UB.

void dump_kwg(KwgNode *kwg, VecChar *word, uint32_t p, Tile tileset[static 1]) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t len = orig_len + strlen(tileset[kwg[p].c].label);
    vecChar_ensure_cap(word, len);
    strcpy(word->ptr + orig_len, tileset[kwg[p].c].label);
    word->len = len;
    if (kwg[p].d) printf("%.*s\n", (int)len, word->ptr);
    if (kwg[p].p) dump_kwg(kwg, word, kwg[p].p, tileset);
    if (kwg[p].e) break;
  }
  word->len = orig_len;
}

bool do_lang_rkwg(char **argv, Tile tileset[static 1]) {
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
  KwgNode *kwg = (KwgNode *)file_content;
  VecChar word = vecChar_new(); defer_free_word = true;
  dump_kwg(kwg, &word, kwg[0].p, tileset);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

bool do_lang_rkwg_gaddag(char **argv, Tile tileset[static 1]) {
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
  KwgNode *kwg = (KwgNode *)file_content;
  VecChar word = vecChar_new(); defer_free_word = true;
  dump_kwg(kwg, &word, kwg[1].p, tileset);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

void dump_klv2(KwgNode *kwg, VecChar *word, uint32_t p, Tile tileset[static 1], float **klv_ptr) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t len = orig_len + strlen(tileset[kwg[p].c].label);
    vecChar_ensure_cap(word, len);
    strcpy(word->ptr + orig_len, tileset[kwg[p].c].label);
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

bool do_lang_rklv2(char **argv, Tile tileset[static 1]) {
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
  KwgNode *kwg = (KwgNode *)(file_content + sizeof(uint32_t));
  float *klv_values = (float *)(file_content + sizeof(uint32_t) * (2 + num_kwg_nodes));
  VecChar word = vecChar_new(); defer_free_word = true;
  dump_klv2(kwg, &word, kwg[0].p, tileset, &klv_values);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

bool do_lang(int argc, char **argv, const char lang_name[static 1], ParsedTile tileset_parse(uint8_t *), Tile tileset[static 1]) {
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
  } else if (!strcmp(argv[1] + lang_name_len, "-read-kwg-gaddag")) {
    if (argc < 3) goto needs_more_args;
    time_goes_to_stderr = true;
    return do_lang_rkwg_gaddag(argv, tileset);
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
      "  english-read-kwg infile.kwg\n"
      "    read kwg on a little-endian system (dawg part only)\n"
      "  english-read-kwg-gaddag infile.kwg\n"
      "    read gaddag part of kwg on a little-endian system\n"
      "  english-read-klv2 infile.klv2\n"
      "    read klv2 on a little-endian system\n"
      "  (english can also be catalan, french, german, norwegian, polish, slovene,\n"
      "    spanish, yupik)");
  }
}
