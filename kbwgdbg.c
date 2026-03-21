// Copyright (C) 2020-2026 Andy Kurnia.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

typedef struct { uint8_t c : 6; bool e : 1, d : 1; uint32_t p : 24; } KbwgNode; // compiler-specific UB.

#define XWGDBG_NODE_T KbwgNode
#define XWGDBG_EXT "kbwg"
#include "generic_xwgdbg.c"
#undef XWGDBG_EXT
#undef XWGDBG_NODE_T
