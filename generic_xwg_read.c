// Copyright (C) 2020-2026 Andy Kurnia.

// usage:
// define XWG_NODE_T KwgNode
// define XWG_NAME kwg
// include
// undef XWG_NAME
// undef XWG_NODE_T

#define XWG_DUMP GENERIC_CONCAT(dump_, XWG_NAME)
#define XWG_DO_LANG GENERIC_CONCAT(do_lang_r, XWG_NAME)

void XWG_DUMP(XWG_NODE_T *nodes, VecChar *word, uint32_t p, Tile tileset[static 1]) {
  size_t orig_len = word->len;
  for (; p > 0; ++p) {
    size_t label_len = strlen(tileset[nodes[p].c].label);
    size_t len = orig_len + label_len;
    vecChar_ensure_cap(word, len);
    memcpy(word->ptr + orig_len, tileset[nodes[p].c].label, label_len);
    word->len = len;
    if (nodes[p].d) printf("%.*s\n", (int)len, word->ptr);
    if (nodes[p].p) XWG_DUMP(nodes, word, nodes[p].p, tileset);
    if (nodes[p].e) break;
  }
  word->len = orig_len;
}

bool XWG_DO_LANG(const char *in, Tile tileset[static 1], ptrdiff_t root) {
  bool errored = false;
  bool defer_free_file_content = false;
  bool defer_free_word = false;
  size_t file_size;
  uint8_t *file_content = read_all(in, &file_size, 0); if (!file_content) goto errored; defer_free_file_content = true;
  if (is_big_endian()) swap_bytes_32(file_content, file_size);
  XWG_NODE_T *nodes = (XWG_NODE_T *)file_content;
  VecChar word = vecChar_new(); defer_free_word = true;
  XWG_DUMP(nodes, &word, nodes[root].p, tileset);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  return !errored;
}

#undef XWG_DO_LANG
#undef XWG_DUMP
