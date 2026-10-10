// Copyright (C) 2020-2026 Andy Kurnia.

// usage:
// define XWG_NODE_T KwgNode
// define XWG_NAME kwg
// include
// undef XWG_NAME
// undef XWG_NODE_T

#define XWG_DUMP GENERIC_CONCAT(dump_, XWG_NAME)
#define XWG_HOLDS_TILE_0 GENERIC_CONCAT(holds_tile_0_, XWG_NAME)
#define XWG_COUNT_WORDS_AT GENERIC_CONCAT(count_words_at_, XWG_NAME)
#define XWG_CHECK GENERIC_CONCAT(check_, XWG_NAME)
#define XWG_DO_LANG GENERIC_CONCAT(do_lang_r, XWG_NAME)

// tile 0 is written zero_label: under the gaddag's root it is the turnaround,
// "@" as read writes it; under the dawg's root it is the blank, by its label.
// a list is taken in tile order, as read takes it, whatever order a reordered
// graph holds it in
void XWG_DUMP(XWG_NODE_T *nodes, VecChar *word, uint32_t first_p, Tile tileset[static 1], const char *zero_label) {
  if (!first_p) return;
  size_t orig_len = word->len;
  for (int prev_tile = -1; ; ) {
    uint32_t p = 0;
    for (uint32_t q = first_p; ; ++q) {
      if (nodes[q].c > prev_tile && (!p || nodes[q].c < nodes[p].c)) p = q;
      if (nodes[q].e) break;
    }
    if (!p) break;
    prev_tile = nodes[p].c;
    const char *label = tileset[nodes[p].c].label;
    if (!nodes[p].c) label = zero_label;
    size_t label_len = strlen(label);
    size_t len = orig_len + label_len;
    vecChar_ensure_cap(word, len);
    memcpy(word->ptr + orig_len, label, label_len);
    word->len = len;
    if (nodes[p].d) printf("%.*s\n", (int)len, word->ptr);
    if (nodes[p].p) XWG_DUMP(nodes, word, nodes[p].p, tileset, zero_label);
  }
  word->len = orig_len;
}

// whether the list at p, or one under it, holds tile 0; each list once
static bool XWG_HOLDS_TILE_0(XWG_NODE_T *nodes, uint32_t p, bool seen[static 1]) {
  if (seen[p]) return false;
  seen[p] = true;
  for (; ; ++p) {
    if (!nodes[p].c) return true;
    if (nodes[p].p && XWG_HOLDS_TILE_0(nodes, nodes[p].p, seen)) return true;
    if (nodes[p].e) return false;
  }
}

// the words from p, as wolges counts them (kwg.rs count_words_at): each node
// once, and a node met again while it is being counted is a loop
static bool XWG_COUNT_WORDS_AT(XWG_NODE_T *nodes, uint32_t word_counts[static 1], size_t num_nodes, size_t p, uint32_t count[static 1]) {
  if (p >= num_nodes) {
    *count = 0;
    return true;
  }
  if (word_counts[p] == (uint32_t)~0) {
    fprintf(stderr, "the word graph loops back to node %zu\n", p);
    return false;
  }
  if (!word_counts[p]) {
    word_counts[p] = (uint32_t)~0; // being counted
    uint32_t arc_count = 0, next_count = 0;
    if (nodes[p].p && !XWG_COUNT_WORDS_AT(nodes, word_counts, num_nodes, nodes[p].p, &arc_count)) return false;
    if (!nodes[p].e && !XWG_COUNT_WORDS_AT(nodes, word_counts, num_nodes, p + 1, &next_count)) return false;
    uint64_t sum = (uint64_t)nodes[p].d + arc_count + next_count;
    if (sum >= (uint32_t)~0) {
      fputs("the word graph holds too many words to count\n", stderr);
      return false;
    }
    word_counts[p] = (uint32_t)sum;
  }
  *count = word_counts[p];
  return true;
}

// a word graph as wolges loads one before read reads it (kwg.rs
// from_bytes_alloc, count_words_alloc): whole nodes, at least one; every arc
// inside; the last node ends its run; no walk loops. its word counts go to
// *counts if counts is not NULL, for the caller to free.
static bool XWG_CHECK(XWG_NODE_T *nodes, size_t file_size, uint32_t **counts) {
  if (!file_size || file_size % sizeof(XWG_NODE_T)) {
    fprintf(stderr, "a word graph of %zu bytes is not whole nodes\n", file_size);
    return false;
  }
  size_t num_nodes = file_size / sizeof(XWG_NODE_T);
  uint32_t max_arc_index = 0;
  for (size_t p = 0; p < num_nodes; ++p) {
    if (nodes[p].p > max_arc_index) max_arc_index = nodes[p].p;
  }
  if (max_arc_index >= num_nodes) {
    fprintf(stderr, "the word graph points to node %u, past its %zu nodes\n", max_arc_index, num_nodes);
    return false;
  }
  if (!nodes[num_nodes - 1].e) {
    fputs("the word graph's last node does not end its run\n", stderr);
    return false;
  }
  uint32_t *word_counts = malloc_or_die(num_nodes * sizeof(uint32_t));
  memset(word_counts, 0, num_nodes * sizeof(uint32_t));
  bool ok = true;
  for (size_t p = num_nodes; ok && p-- > 0; ) {
    uint32_t count;
    ok = XWG_COUNT_WORDS_AT(nodes, word_counts, num_nodes, p, &count);
  }
  if (ok && counts) {
    *counts = word_counts;
  } else {
    free(word_counts);
  }
  return ok;
}

bool XWG_DO_LANG(const char *in, Tile tileset[static 1], ptrdiff_t root, bool blanks) {
  bool errored = false;
  bool defer_free_file_content = false;
  bool defer_free_seen = false;
  bool defer_free_word = false;
  size_t file_size;
  uint8_t *file_content = read_all(in, &file_size, 0); if (!file_content) goto errored; defer_free_file_content = true;
  if (is_big_endian()) swap_bytes_32(file_content, file_size);
  XWG_NODE_T *nodes = (XWG_NODE_T *)file_content;
  if (!XWG_CHECK(nodes, file_size, NULL)) goto errored;
  size_t num_nodes = file_size / sizeof(XWG_NODE_T);
  // a graph of one node (an empty dawg) has no node 1, so no gaddag; read
  // refuses to read one too
  if ((size_t)root >= num_nodes) {
    fputs("the word graph has no gaddag: it is one node\n", stderr);
    goto errored;
  }
  // under the dawg's root tile 0 is a blank, read only with --blanks (read's
  // rule), so it is refused before a word is written
  bool *seen = NULL;
  if (!root && !blanks && nodes[0].p) {
    seen = malloc_or_die(num_nodes * sizeof(bool)); defer_free_seen = true;
    memset(seen, 0, num_nodes * sizeof(bool));
    if (XWG_HOLDS_TILE_0(nodes, nodes[0].p, seen)) {
      fputs("the dawg holds a blank (tile 0): read it with --blanks\n", stderr);
      goto errored;
    }
  }
  VecChar word = vecChar_new(); defer_free_word = true;
  XWG_DUMP(nodes, &word, nodes[root].p, tileset, root ? "@" : tileset[0].label);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_seen) free(seen);
  if (defer_free_file_content) free(file_content);
  return !errored;
}

#undef XWG_DO_LANG
#undef XWG_CHECK
#undef XWG_COUNT_WORDS_AT
#undef XWG_HOLDS_TILE_0
#undef XWG_DUMP
