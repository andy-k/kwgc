// Copyright (C) 2020-2026 Andy Kurnia.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

typedef struct { uint8_t c : 6; bool e : 1, d : 1; uint32_t p : 24; } KbwgNode; // compiler-specific UB.

static inline void *not_null_or_die(void *ptr) {
  if (!ptr) { perror("not_null_or_die"); abort(); }
  return ptr;
}

static inline void *realloc_or_die(void *ptr, size_t size) {
  return not_null_or_die(realloc(ptr, size));
}

#define VEC_ELT_NAME Byte
#define VEC_ELT_T uint8_t
#include "generic_vec.c"
#undef VEC_ELT_T
#undef VEC_ELT_NAME

void dump_kbwg(KbwgNode *kbwg, VecByte *buf, size_t l, uint32_t p) {
  for (; p > 0; ++p) {
    vecByte_ensure_cap(buf, l + 1);
    buf->ptr[l] = kbwg[p].c | 0x40; // english only.
    if (kbwg[p].d) printf("%.*s\n", (int)(l + 1), (char *)buf->ptr);
    if (kbwg[p].p) dump_kbwg(kbwg, buf, l + 1, kbwg[p].p);
    if (kbwg[p].e) break;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: %s something.kbwg [n]\nn = 0 for dawg, 1 for gaddag\n", argv[0]); return 0; }
  ptrdiff_t dawgroot = 0;
  char check; // check if the int (which can have 0x prefix) is followed by some junk after whitespace.
  if (argc > 2) { if (sscanf(argv[2], "%zi %c", &dawgroot, &check) != 1) { fputs("put a number as second arg\n", stderr); return 0; } }
  bool errored = false;
  bool defer_fclose = false;
  bool defer_munmap = false;
  FILE *f = fopen(argv[1], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t kbwg_size_signed = ftello(f); if (kbwg_size_signed < 0) { perror("ftello"); goto errored; }
  size_t kbwg_size = (size_t)kbwg_size_signed;
  if ((kbwg_size & 3) != 0 || !((size_t)dawgroot < (kbwg_size >> 2))) { fputs("unexpected file size\n", stderr); goto errored; }
  KbwgNode *kbwg = mmap(NULL, kbwg_size, PROT_READ, MAP_SHARED, fileno(f), 0); if (kbwg == MAP_FAILED) { perror("mmap"); goto errored; } defer_munmap = true;
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  VecByte buf = vecByte_new();
  dump_kbwg(kbwg, &buf, 0, kbwg[dawgroot].p);
  vecByte_free(&buf);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_munmap) { if (munmap(kbwg, kbwg_size)) { perror("munmap"); errored = true; } }
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return errored;
}
