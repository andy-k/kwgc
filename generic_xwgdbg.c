// Copyright (C) 2020-2026 Andy Kurnia.

// usage:
// define XWGDBG_NODE_T KwgNode
// define XWGDBG_EXT "kwg"
// include
// undef XWGDBG_EXT
// undef XWGDBG_NODE_T

#include "alloc.c"

#define VEC_ELT_NAME Byte
#define VEC_ELT_T uint8_t
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

static void dump(XWGDBG_NODE_T *nodes, VecByte *buf, size_t l, uint32_t p) {
  for (; p > 0; ++p) {
    vecByte_ensure_cap(buf, l + 1);
    buf->ptr[l] = nodes[p].c | 0x40; // english only.
    if (nodes[p].d) printf("%.*s\n", (int)(l + 1), (char *)buf->ptr);
    if (nodes[p].p) dump(nodes, buf, l + 1, nodes[p].p);
    if (nodes[p].e) break;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: %s something." XWGDBG_EXT " [n]\nn = 0 for dawg, 1 for gaddag\n", argv[0]); return 0; }
  ptrdiff_t dawgroot = 0;
  char check; // check if the int (which can have 0x prefix) is followed by some junk after whitespace.
  if (argc > 2) { if (sscanf(argv[2], "%zi %c", &dawgroot, &check) != 1) { fputs("put a number as second arg\n", stderr); return 0; } }
  bool errored = false;
  bool defer_fclose = false;
  bool defer_munmap = false;
  FILE *f = fopen(argv[1], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t file_size_signed = ftello(f); if (file_size_signed < 0) { perror("ftello"); goto errored; }
  size_t file_size = (size_t)file_size_signed;
  if ((file_size & 3) != 0 || !((size_t)dawgroot < (file_size >> 2))) { fputs("unexpected file size\n", stderr); goto errored; }
  XWGDBG_NODE_T *nodes = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fileno(f), 0); if (nodes == MAP_FAILED) { perror("mmap"); goto errored; } defer_munmap = true;
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  VecByte buf = vecByte_new();
  dump(nodes, &buf, 0, nodes[dawgroot].p);
  vecByte_free(&buf);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_munmap) { if (munmap(nodes, file_size)) { perror("munmap"); errored = true; } }
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return errored;
}
