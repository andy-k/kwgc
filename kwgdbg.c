// Copyright (C) 2020-2026 Andy Kurnia.

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

typedef struct { uint32_t p : 22; bool e : 1, d : 1; uint8_t c : 8; } KwgNode; // compiler-specific UB.

#define XWGDBG_NODE_T KwgNode
#define XWGDBG_EXT "kwg"
#include "generic_xwgdbg.c"
#undef XWGDBG_EXT
#undef XWGDBG_NODE_T
