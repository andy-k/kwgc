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

bool XWG_DO_LANG(char **argv, Tile tileset[static 1], ptrdiff_t root) {
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
  XWG_NODE_T *nodes = (XWG_NODE_T *)file_content;
  VecChar word = vecChar_new(); defer_free_word = true;
  XWG_DUMP(nodes, &word, nodes[root].p, tileset);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_free_word) vecChar_free(&word);
  if (defer_free_file_content) free(file_content);
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return !errored;
}

#undef XWG_DO_LANG
#undef XWG_DUMP
