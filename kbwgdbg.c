// Copyright (C) 2020-2025 Andy Kurnia.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

typedef struct { uint8_t c : 6; bool e : 1, d : 1; uint32_t p : 24; } KbwgNode; // compiler-specific UB.

void dump_kbwg(KbwgNode *kbwg, char s[static 1], size_t l, uint32_t p) {
  for (; p > 0; ++p) {
    s[l] = (char)(kbwg[p].c | 0x40); // english only.
    if (kbwg[p].d) printf("%.*s\n", (int)(l + 1), s);
    if (kbwg[p].p) dump_kbwg(kbwg, s, l + 1, kbwg[p].p);
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
  KbwgNode *kbwg = mmap(NULL, kbwg_size, PROT_READ, MAP_SHARED, fileno(f), 0); if (!kbwg) { perror("mmap"); goto errored; } defer_munmap = true;
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  char buf[64]; // risky!
  dump_kbwg(kbwg, buf, 0, kbwg[dawgroot].p);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_munmap) { if (munmap(kbwg, kbwg_size)) { perror("munmap"); errored = true; } }
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return errored;
}
