// Copyright (C) 2020-2026 Andy Kurnia.

// usage:
// define XWG_NODE_T KwgNode
// define XWG_NAME kwg
// include
// undef XWG_NAME
// undef XWG_NODE_T

#define XWG_COUNT_WORDS_AT GENERIC_CONCAT(count_words_at_, XWG_NAME)
#define XWG_CHECK GENERIC_CONCAT(check_, XWG_NAME)

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

#undef XWG_CHECK
#undef XWG_COUNT_WORDS_AT
