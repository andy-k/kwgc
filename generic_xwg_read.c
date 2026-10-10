// Copyright (C) 2020-2026 Andy Kurnia.

// usage:
// define XWG_NODE_T KwgNode
// define XWG_NAME kwg
// include
// undef XWG_NAME
// undef XWG_NODE_T

#include "generic_xwg_check.c"

#define XWG_DUMP GENERIC_CONCAT(dump_, XWG_NAME)
#define XWG_HOLDS_TILE_0 GENERIC_CONCAT(holds_tile_0_, XWG_NAME)
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
#undef XWG_HOLDS_TILE_0
#undef XWG_DUMP
