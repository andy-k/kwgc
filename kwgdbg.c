// Copyright (C) 2020-2024 Andy Kurnia.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

typedef struct { uint32_t p : 22; bool e : 1, d : 1; uint8_t c : 8; } KwgNode; // compiler-specific UB.

void dump_kwg(KwgNode *kwg, char s[static 1], size_t l, uint32_t p) {
  for (; p > 0; ++p) {
    s[l] = (char)(kwg[p].c | 0x40); // english only.
    if (kwg[p].d) printf("%.*s\n", (int)(l + 1), s);
    if (kwg[p].p) dump_kwg(kwg, s, l + 1, kwg[p].p);
    if (kwg[p].e) break;
  }
}

int main(int argc, char **argv) {
  if (argc < 2) { fprintf(stderr, "usage: %s something.kwg [n]\nn = 0 for dawg, 1 for gaddag\n", argv[0]); return 0; }
  ptrdiff_t dawgroot = 0;
  char check; // check if the int (which can have 0x prefix) is followed by some junk after whitespace.
  if (argc > 2) { if (sscanf(argv[2], "%zi %c", &dawgroot, &check) != 1) { fputs("put a number as second arg\n", stderr); return 0; } }
  bool errored = false;
  bool defer_fclose = false;
  bool defer_munmap = false;
  FILE *f = fopen(argv[1], "rb"); if (!f) { perror("fopen"); goto errored; } defer_fclose = true;
  if (fseek(f, 0L, SEEK_END)) { perror("fseek"); goto errored; }
  off_t kwg_size_signed = ftello(f); if (kwg_size_signed < 0) { perror("ftello"); goto errored; }
  size_t kwg_size = (size_t)kwg_size_signed;
  if ((kwg_size & 3) != 0 || !((size_t)dawgroot < (kwg_size >> 2))) { fputs("unexpected file size\n", stderr); goto errored; }
  KwgNode *kwg = mmap(NULL, kwg_size, PROT_READ, MAP_SHARED, fileno(f), 0); if (!kwg) { perror("mmap"); goto errored; } defer_munmap = true;
  defer_fclose = false; if (fclose(f)) { perror("fclose"); goto errored; }
  char buf[64]; // risky!
  dump_kwg(kwg, buf, 0, kwg[dawgroot].p);
  goto cleanup;
errored: errored = true;
cleanup:
  if (defer_munmap) { if (munmap(kwg, kwg_size)) { perror("munmap"); errored = true; } }
  if (defer_fclose) { if (fclose(f)) { perror("fclose"); errored = true; } }
  return errored;
}
