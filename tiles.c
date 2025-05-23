// Copyright (C) 2020-2025 Andy Kurnia.

#include <stdlib.h>

// tile-label-specific

typedef struct {
  char *label;
  char *blank_label;
} Tile;

typedef struct {
  size_t len;
  uint8_t index;
} ParsedTile;

Tile catalan_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "B", .blank_label = "b" }, // 2
  { .label = "C", .blank_label = "c" }, // 3
  { .label = "Ç", .blank_label = "ç" }, // 4
  { .label = "D", .blank_label = "d" }, // 5
  { .label = "E", .blank_label = "e" }, // 6
  { .label = "F", .blank_label = "f" }, // 7
  { .label = "G", .blank_label = "g" }, // 8
  { .label = "H", .blank_label = "h" }, // 9
  { .label = "I", .blank_label = "i" }, // 10
  { .label = "J", .blank_label = "j" }, // 11
  { .label = "L", .blank_label = "l" }, // 12
  { .label = "L·L", .blank_label = "l·l" }, // 13
  { .label = "M", .blank_label = "m" }, // 14
  { .label = "N", .blank_label = "n" }, // 15
  { .label = "NY", .blank_label = "ny" }, // 16
  { .label = "O", .blank_label = "o" }, // 17
  { .label = "P", .blank_label = "p" }, // 18
  { .label = "QU", .blank_label = "qu" }, // 19
  { .label = "R", .blank_label = "r" }, // 20
  { .label = "S", .blank_label = "s" }, // 21
  { .label = "T", .blank_label = "t" }, // 22
  { .label = "U", .blank_label = "u" }, // 23
  { .label = "V", .blank_label = "v" }, // 24
  { .label = "X", .blank_label = "x" }, // 25
  { .label = "Z", .blank_label = "z" }, // 26
};

ParsedTile catalan_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 11 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 4 }; // K
  case 0x4c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x4c) return (ParsedTile){ .len = 4, .index = 13 }; // L·L
    return (ParsedTile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    if (ptr[1] == 0x59) return (ParsedTile){ .len = 2, .index = 16 }; // NY
    return (ParsedTile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 17 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 18 }; // P
  case 0x51:
    if (ptr[1] == 0x55) return (ParsedTile){ .len = 2, .index = 19 }; // QU
    return (ParsedTile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 21 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 22 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 23 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 24 }; // V
  case 0x57:
    return (ParsedTile){ .len = 1, .index = 13 }; // W
  case 0x58:
    return (ParsedTile){ .len = 1, .index = 25 }; // X
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 16 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 26 }; // Z
  case 0x5b:
    if (ptr[1] == 0x4c && ptr[2] == 0xc2 && ptr[3] == 0xb7 && ptr[4] == 0x4c && ptr[5] == 0x5d) return (ParsedTile){ .len = 6, .index = 13 }; // [L·L]
    if (ptr[1] == 0x6c && ptr[2] == 0xc2 && ptr[3] == 0xb7 && ptr[4] == 0x6c && ptr[5] == 0x5d) return (ParsedTile){ .len = 6, .index = 13 }; // [l·l]
    if (ptr[1] == 0x4e && ptr[2] == 0x59 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 16 }; // [NY]
    if (ptr[1] == 0x6e && ptr[2] == 0x79 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 16 }; // [ny]
    if (ptr[1] == 0x51 && ptr[2] == 0x55 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 19 }; // [QU]
    if (ptr[1] == 0x71 && ptr[2] == 0x75 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 19 }; // [qu]
    break;
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 11 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 4 }; // k
  case 0x6c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x6c) return (ParsedTile){ .len = 4, .index = 13 }; // l·l
    return (ParsedTile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    if (ptr[1] == 0x79) return (ParsedTile){ .len = 2, .index = 16 }; // ny
    return (ParsedTile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 17 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 18 }; // p
  case 0x71:
    if (ptr[1] == 0x75) return (ParsedTile){ .len = 2, .index = 19 }; // qu
    return (ParsedTile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 21 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 22 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 23 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 24 }; // v
  case 0x77:
    return (ParsedTile){ .len = 1, .index = 13 }; // w
  case 0x78:
    return (ParsedTile){ .len = 1, .index = 25 }; // x
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 16 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 26 }; // z
  case 0xc3:
    if (ptr[1] == 0x87) return (ParsedTile){ .len = 2, .index = 4 }; // Ç
    if (ptr[1] == 0xa7) return (ParsedTile){ .len = 2, .index = 4 }; // ç
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile decimal_tileset[] = {
  { .label = "[0]", .blank_label = "[-0]" }, // 0
  { .label = "[1]", .blank_label = "[-1]" }, // 1
  { .label = "[2]", .blank_label = "[-2]" }, // 2
  { .label = "[3]", .blank_label = "[-3]" }, // 3
  { .label = "[4]", .blank_label = "[-4]" }, // 4
  { .label = "[5]", .blank_label = "[-5]" }, // 5
  { .label = "[6]", .blank_label = "[-6]" }, // 6
  { .label = "[7]", .blank_label = "[-7]" }, // 7
  { .label = "[8]", .blank_label = "[-8]" }, // 8
  { .label = "[9]", .blank_label = "[-9]" }, // 9
  { .label = "[10]", .blank_label = "[-10]" }, // 10
  { .label = "[11]", .blank_label = "[-11]" }, // 11
  { .label = "[12]", .blank_label = "[-12]" }, // 12
  { .label = "[13]", .blank_label = "[-13]" }, // 13
  { .label = "[14]", .blank_label = "[-14]" }, // 14
  { .label = "[15]", .blank_label = "[-15]" }, // 15
  { .label = "[16]", .blank_label = "[-16]" }, // 16
  { .label = "[17]", .blank_label = "[-17]" }, // 17
  { .label = "[18]", .blank_label = "[-18]" }, // 18
  { .label = "[19]", .blank_label = "[-19]" }, // 19
  { .label = "[20]", .blank_label = "[-20]" }, // 20
  { .label = "[21]", .blank_label = "[-21]" }, // 21
  { .label = "[22]", .blank_label = "[-22]" }, // 22
  { .label = "[23]", .blank_label = "[-23]" }, // 23
  { .label = "[24]", .blank_label = "[-24]" }, // 24
  { .label = "[25]", .blank_label = "[-25]" }, // 25
  { .label = "[26]", .blank_label = "[-26]" }, // 26
  { .label = "[27]", .blank_label = "[-27]" }, // 27
  { .label = "[28]", .blank_label = "[-28]" }, // 28
  { .label = "[29]", .blank_label = "[-29]" }, // 29
  { .label = "[30]", .blank_label = "[-30]" }, // 30
  { .label = "[31]", .blank_label = "[-31]" }, // 31
  { .label = "[32]", .blank_label = "[-32]" }, // 32
  { .label = "[33]", .blank_label = "[-33]" }, // 33
  { .label = "[34]", .blank_label = "[-34]" }, // 34
  { .label = "[35]", .blank_label = "[-35]" }, // 35
  { .label = "[36]", .blank_label = "[-36]" }, // 36
  { .label = "[37]", .blank_label = "[-37]" }, // 37
  { .label = "[38]", .blank_label = "[-38]" }, // 38
  { .label = "[39]", .blank_label = "[-39]" }, // 39
  { .label = "[40]", .blank_label = "[-40]" }, // 40
  { .label = "[41]", .blank_label = "[-41]" }, // 41
  { .label = "[42]", .blank_label = "[-42]" }, // 42
  { .label = "[43]", .blank_label = "[-43]" }, // 43
  { .label = "[44]", .blank_label = "[-44]" }, // 44
  { .label = "[45]", .blank_label = "[-45]" }, // 45
  { .label = "[46]", .blank_label = "[-46]" }, // 46
  { .label = "[47]", .blank_label = "[-47]" }, // 47
  { .label = "[48]", .blank_label = "[-48]" }, // 48
  { .label = "[49]", .blank_label = "[-49]" }, // 49
  { .label = "[50]", .blank_label = "[-50]" }, // 50
  { .label = "[51]", .blank_label = "[-51]" }, // 51
  { .label = "[52]", .blank_label = "[-52]" }, // 52
  { .label = "[53]", .blank_label = "[-53]" }, // 53
  { .label = "[54]", .blank_label = "[-54]" }, // 54
  { .label = "[55]", .blank_label = "[-55]" }, // 55
  { .label = "[56]", .blank_label = "[-56]" }, // 56
  { .label = "[57]", .blank_label = "[-57]" }, // 57
  { .label = "[58]", .blank_label = "[-58]" }, // 58
  { .label = "[59]", .blank_label = "[-59]" }, // 59
  { .label = "[60]", .blank_label = "[-60]" }, // 60
  { .label = "[61]", .blank_label = "[-61]" }, // 61
  { .label = "[62]", .blank_label = "[-62]" }, // 62
  { .label = "[63]", .blank_label = "[-63]" }, // 63
};

ParsedTile decimal_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x5b:
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 10 }; // [-10]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 11 }; // [-11]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 12 }; // [-12]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 13 }; // [-13]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x34 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 14 }; // [-14]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x35 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 15 }; // [-15]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x36 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 16 }; // [-16]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x37 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 17 }; // [-17]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x38 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 18 }; // [-18]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x39 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 19 }; // [-19]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 20 }; // [-20]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 21 }; // [-21]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 22 }; // [-22]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 23 }; // [-23]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x34 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 24 }; // [-24]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x35 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 25 }; // [-25]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x36 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 26 }; // [-26]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x37 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 27 }; // [-27]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x38 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 28 }; // [-28]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x39 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 29 }; // [-29]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 30 }; // [-30]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 31 }; // [-31]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 32 }; // [-32]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 33 }; // [-33]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x34 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 34 }; // [-34]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x35 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 35 }; // [-35]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x36 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 36 }; // [-36]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x37 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 37 }; // [-37]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x38 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 38 }; // [-38]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x39 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 39 }; // [-39]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 40 }; // [-40]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 41 }; // [-41]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 42 }; // [-42]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 43 }; // [-43]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x34 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 44 }; // [-44]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x35 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 45 }; // [-45]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x36 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 46 }; // [-46]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x37 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 47 }; // [-47]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x38 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 48 }; // [-48]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x39 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 49 }; // [-49]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 50 }; // [-50]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 51 }; // [-51]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 52 }; // [-52]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 53 }; // [-53]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x34 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 54 }; // [-54]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x35 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 55 }; // [-55]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x36 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 56 }; // [-56]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x37 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 57 }; // [-57]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x38 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 58 }; // [-58]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x39 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 59 }; // [-59]
    if (ptr[1] == 0x2d && ptr[2] == 0x36 && ptr[3] == 0x30 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 60 }; // [-60]
    if (ptr[1] == 0x2d && ptr[2] == 0x36 && ptr[3] == 0x31 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 61 }; // [-61]
    if (ptr[1] == 0x2d && ptr[2] == 0x36 && ptr[3] == 0x32 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 62 }; // [-62]
    if (ptr[1] == 0x2d && ptr[2] == 0x36 && ptr[3] == 0x33 && ptr[4] == 0x5d) return (ParsedTile){ .len = 5, .index = 63 }; // [-63]
    if (ptr[1] == 0x2d && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 0 }; // [-0]
    if (ptr[1] == 0x2d && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 1 }; // [-1]
    if (ptr[1] == 0x2d && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 2 }; // [-2]
    if (ptr[1] == 0x2d && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 3 }; // [-3]
    if (ptr[1] == 0x2d && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 4 }; // [-4]
    if (ptr[1] == 0x2d && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 5 }; // [-5]
    if (ptr[1] == 0x2d && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 6 }; // [-6]
    if (ptr[1] == 0x2d && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 7 }; // [-7]
    if (ptr[1] == 0x2d && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 8 }; // [-8]
    if (ptr[1] == 0x2d && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 9 }; // [-9]
    if (ptr[1] == 0x31 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 10 }; // [10]
    if (ptr[1] == 0x31 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 11 }; // [11]
    if (ptr[1] == 0x31 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 12 }; // [12]
    if (ptr[1] == 0x31 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 13 }; // [13]
    if (ptr[1] == 0x31 && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 14 }; // [14]
    if (ptr[1] == 0x31 && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 15 }; // [15]
    if (ptr[1] == 0x31 && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 16 }; // [16]
    if (ptr[1] == 0x31 && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 17 }; // [17]
    if (ptr[1] == 0x31 && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 18 }; // [18]
    if (ptr[1] == 0x31 && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 19 }; // [19]
    if (ptr[1] == 0x32 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 20 }; // [20]
    if (ptr[1] == 0x32 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 21 }; // [21]
    if (ptr[1] == 0x32 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 22 }; // [22]
    if (ptr[1] == 0x32 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 23 }; // [23]
    if (ptr[1] == 0x32 && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 24 }; // [24]
    if (ptr[1] == 0x32 && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 25 }; // [25]
    if (ptr[1] == 0x32 && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 26 }; // [26]
    if (ptr[1] == 0x32 && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 27 }; // [27]
    if (ptr[1] == 0x32 && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 28 }; // [28]
    if (ptr[1] == 0x32 && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 29 }; // [29]
    if (ptr[1] == 0x33 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 30 }; // [30]
    if (ptr[1] == 0x33 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 31 }; // [31]
    if (ptr[1] == 0x33 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 32 }; // [32]
    if (ptr[1] == 0x33 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 33 }; // [33]
    if (ptr[1] == 0x33 && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 34 }; // [34]
    if (ptr[1] == 0x33 && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 35 }; // [35]
    if (ptr[1] == 0x33 && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 36 }; // [36]
    if (ptr[1] == 0x33 && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 37 }; // [37]
    if (ptr[1] == 0x33 && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 38 }; // [38]
    if (ptr[1] == 0x33 && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 39 }; // [39]
    if (ptr[1] == 0x34 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 40 }; // [40]
    if (ptr[1] == 0x34 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 41 }; // [41]
    if (ptr[1] == 0x34 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 42 }; // [42]
    if (ptr[1] == 0x34 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 43 }; // [43]
    if (ptr[1] == 0x34 && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 44 }; // [44]
    if (ptr[1] == 0x34 && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 45 }; // [45]
    if (ptr[1] == 0x34 && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 46 }; // [46]
    if (ptr[1] == 0x34 && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 47 }; // [47]
    if (ptr[1] == 0x34 && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 48 }; // [48]
    if (ptr[1] == 0x34 && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 49 }; // [49]
    if (ptr[1] == 0x35 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 50 }; // [50]
    if (ptr[1] == 0x35 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 51 }; // [51]
    if (ptr[1] == 0x35 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 52 }; // [52]
    if (ptr[1] == 0x35 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 53 }; // [53]
    if (ptr[1] == 0x35 && ptr[2] == 0x34 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 54 }; // [54]
    if (ptr[1] == 0x35 && ptr[2] == 0x35 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 55 }; // [55]
    if (ptr[1] == 0x35 && ptr[2] == 0x36 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 56 }; // [56]
    if (ptr[1] == 0x35 && ptr[2] == 0x37 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 57 }; // [57]
    if (ptr[1] == 0x35 && ptr[2] == 0x38 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 58 }; // [58]
    if (ptr[1] == 0x35 && ptr[2] == 0x39 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 59 }; // [59]
    if (ptr[1] == 0x36 && ptr[2] == 0x30 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 60 }; // [60]
    if (ptr[1] == 0x36 && ptr[2] == 0x31 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 61 }; // [61]
    if (ptr[1] == 0x36 && ptr[2] == 0x32 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 62 }; // [62]
    if (ptr[1] == 0x36 && ptr[2] == 0x33 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 63 }; // [63]
    if (ptr[1] == 0x30 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 0 }; // [0]
    if (ptr[1] == 0x31 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 1 }; // [1]
    if (ptr[1] == 0x32 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 2 }; // [2]
    if (ptr[1] == 0x33 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 3 }; // [3]
    if (ptr[1] == 0x34 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 4 }; // [4]
    if (ptr[1] == 0x35 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 5 }; // [5]
    if (ptr[1] == 0x36 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 6 }; // [6]
    if (ptr[1] == 0x37 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 7 }; // [7]
    if (ptr[1] == 0x38 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 8 }; // [8]
    if (ptr[1] == 0x39 && ptr[2] == 0x5d) return (ParsedTile){ .len = 3, .index = 9 }; // [9]
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile dutch_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "B", .blank_label = "b" }, // 2
  { .label = "C", .blank_label = "c" }, // 3
  { .label = "D", .blank_label = "d" }, // 4
  { .label = "E", .blank_label = "e" }, // 5
  { .label = "F", .blank_label = "f" }, // 6
  { .label = "G", .blank_label = "g" }, // 7
  { .label = "H", .blank_label = "h" }, // 8
  { .label = "I", .blank_label = "i" }, // 9
  { .label = "J", .blank_label = "j" }, // 10
  { .label = "K", .blank_label = "k" }, // 11
  { .label = "L", .blank_label = "l" }, // 12
  { .label = "M", .blank_label = "m" }, // 13
  { .label = "N", .blank_label = "n" }, // 14
  { .label = "O", .blank_label = "o" }, // 15
  { .label = "P", .blank_label = "p" }, // 16
  { .label = "Q", .blank_label = "q" }, // 17
  { .label = "R", .blank_label = "r" }, // 18
  { .label = "S", .blank_label = "s" }, // 19
  { .label = "T", .blank_label = "t" }, // 20
  { .label = "U", .blank_label = "u" }, // 21
  { .label = "V", .blank_label = "v" }, // 22
  { .label = "W", .blank_label = "w" }, // 23
  { .label = "X", .blank_label = "x" }, // 24
  { .label = "Y", .blank_label = "y" }, // 25
  { .label = "Z", .blank_label = "z" }, // 26
};

ParsedTile dutch_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 4 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 5 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 6 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 7 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 8 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 9 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 10 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 11 }; // K
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 13 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 14 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 15 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 16 }; // P
  case 0x51:
    return (ParsedTile){ .len = 1, .index = 17 }; // Q
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 18 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 19 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 20 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 21 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 22 }; // V
  case 0x57:
    return (ParsedTile){ .len = 1, .index = 23 }; // W
  case 0x58:
    return (ParsedTile){ .len = 1, .index = 24 }; // X
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 25 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 26 }; // Z
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 4 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 5 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 6 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 7 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 8 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 9 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 10 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 11 }; // k
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 13 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 14 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 15 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 16 }; // p
  case 0x71:
    return (ParsedTile){ .len = 1, .index = 17 }; // q
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 18 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 19 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 20 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 21 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 22 }; // v
  case 0x77:
    return (ParsedTile){ .len = 1, .index = 23 }; // w
  case 0x78:
    return (ParsedTile){ .len = 1, .index = 24 }; // x
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 25 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 26 }; // z
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile hex_tileset[] = {
  { .label = "00", .blank_label = "80" }, // 0
  { .label = "01", .blank_label = "81" }, // 1
  { .label = "02", .blank_label = "82" }, // 2
  { .label = "03", .blank_label = "83" }, // 3
  { .label = "04", .blank_label = "84" }, // 4
  { .label = "05", .blank_label = "85" }, // 5
  { .label = "06", .blank_label = "86" }, // 6
  { .label = "07", .blank_label = "87" }, // 7
  { .label = "08", .blank_label = "88" }, // 8
  { .label = "09", .blank_label = "89" }, // 9
  { .label = "0a", .blank_label = "8a" }, // 10
  { .label = "0b", .blank_label = "8b" }, // 11
  { .label = "0c", .blank_label = "8c" }, // 12
  { .label = "0d", .blank_label = "8d" }, // 13
  { .label = "0e", .blank_label = "8e" }, // 14
  { .label = "0f", .blank_label = "8f" }, // 15
  { .label = "10", .blank_label = "90" }, // 16
  { .label = "11", .blank_label = "91" }, // 17
  { .label = "12", .blank_label = "92" }, // 18
  { .label = "13", .blank_label = "93" }, // 19
  { .label = "14", .blank_label = "94" }, // 20
  { .label = "15", .blank_label = "95" }, // 21
  { .label = "16", .blank_label = "96" }, // 22
  { .label = "17", .blank_label = "97" }, // 23
  { .label = "18", .blank_label = "98" }, // 24
  { .label = "19", .blank_label = "99" }, // 25
  { .label = "1a", .blank_label = "9a" }, // 26
  { .label = "1b", .blank_label = "9b" }, // 27
  { .label = "1c", .blank_label = "9c" }, // 28
  { .label = "1d", .blank_label = "9d" }, // 29
  { .label = "1e", .blank_label = "9e" }, // 30
  { .label = "1f", .blank_label = "9f" }, // 31
  { .label = "20", .blank_label = "a0" }, // 32
  { .label = "21", .blank_label = "a1" }, // 33
  { .label = "22", .blank_label = "a2" }, // 34
  { .label = "23", .blank_label = "a3" }, // 35
  { .label = "24", .blank_label = "a4" }, // 36
  { .label = "25", .blank_label = "a5" }, // 37
  { .label = "26", .blank_label = "a6" }, // 38
  { .label = "27", .blank_label = "a7" }, // 39
  { .label = "28", .blank_label = "a8" }, // 40
  { .label = "29", .blank_label = "a9" }, // 41
  { .label = "2a", .blank_label = "aa" }, // 42
  { .label = "2b", .blank_label = "ab" }, // 43
  { .label = "2c", .blank_label = "ac" }, // 44
  { .label = "2d", .blank_label = "ad" }, // 45
  { .label = "2e", .blank_label = "ae" }, // 46
  { .label = "2f", .blank_label = "af" }, // 47
  { .label = "30", .blank_label = "b0" }, // 48
  { .label = "31", .blank_label = "b1" }, // 49
  { .label = "32", .blank_label = "b2" }, // 50
  { .label = "33", .blank_label = "b3" }, // 51
  { .label = "34", .blank_label = "b4" }, // 52
  { .label = "35", .blank_label = "b5" }, // 53
  { .label = "36", .blank_label = "b6" }, // 54
  { .label = "37", .blank_label = "b7" }, // 55
  { .label = "38", .blank_label = "b8" }, // 56
  { .label = "39", .blank_label = "b9" }, // 57
  { .label = "3a", .blank_label = "ba" }, // 58
  { .label = "3b", .blank_label = "bb" }, // 59
  { .label = "3c", .blank_label = "bc" }, // 60
  { .label = "3d", .blank_label = "bd" }, // 61
  { .label = "3e", .blank_label = "be" }, // 62
  { .label = "3f", .blank_label = "bf" }, // 63
};

ParsedTile hex_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x30:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 0 }; // 00
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 1 }; // 01
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 2 }; // 02
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 3 }; // 03
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 4 }; // 04
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 5 }; // 05
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 6 }; // 06
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 7 }; // 07
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 8 }; // 08
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 9 }; // 09
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 10 }; // 0a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 11 }; // 0b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 12 }; // 0c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 13 }; // 0d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 14 }; // 0e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 15 }; // 0f
    break;
  case 0x31:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 16 }; // 10
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 17 }; // 11
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 18 }; // 12
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 19 }; // 13
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 20 }; // 14
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 21 }; // 15
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 22 }; // 16
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 23 }; // 17
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 24 }; // 18
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 25 }; // 19
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 26 }; // 1a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 27 }; // 1b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 28 }; // 1c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 29 }; // 1d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 30 }; // 1e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 31 }; // 1f
    break;
  case 0x32:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 32 }; // 20
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 33 }; // 21
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 34 }; // 22
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 35 }; // 23
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 36 }; // 24
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 37 }; // 25
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 38 }; // 26
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 39 }; // 27
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 40 }; // 28
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 41 }; // 29
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 42 }; // 2a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 43 }; // 2b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 44 }; // 2c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 45 }; // 2d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 46 }; // 2e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 47 }; // 2f
    break;
  case 0x33:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 48 }; // 30
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 49 }; // 31
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 50 }; // 32
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 51 }; // 33
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 52 }; // 34
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 53 }; // 35
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 54 }; // 36
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 55 }; // 37
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 56 }; // 38
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 57 }; // 39
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 58 }; // 3a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 59 }; // 3b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 60 }; // 3c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 61 }; // 3d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 62 }; // 3e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 63 }; // 3f
    break;
  case 0x38:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 0 }; // 80
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 1 }; // 81
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 2 }; // 82
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 3 }; // 83
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 4 }; // 84
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 5 }; // 85
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 6 }; // 86
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 7 }; // 87
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 8 }; // 88
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 9 }; // 89
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 10 }; // 8a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 11 }; // 8b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 12 }; // 8c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 13 }; // 8d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 14 }; // 8e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 15 }; // 8f
    break;
  case 0x39:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 16 }; // 90
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 17 }; // 91
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 18 }; // 92
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 19 }; // 93
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 20 }; // 94
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 21 }; // 95
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 22 }; // 96
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 23 }; // 97
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 24 }; // 98
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 25 }; // 99
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 26 }; // 9a
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 27 }; // 9b
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 28 }; // 9c
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 29 }; // 9d
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 30 }; // 9e
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 31 }; // 9f
    break;
  case 0x61:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 32 }; // a0
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 33 }; // a1
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 34 }; // a2
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 35 }; // a3
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 36 }; // a4
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 37 }; // a5
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 38 }; // a6
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 39 }; // a7
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 40 }; // a8
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 41 }; // a9
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 42 }; // aa
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 43 }; // ab
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 44 }; // ac
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 45 }; // ad
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 46 }; // ae
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 47 }; // af
    break;
  case 0x62:
    if (ptr[1] == 0x30) return (ParsedTile){ .len = 2, .index = 48 }; // b0
    if (ptr[1] == 0x31) return (ParsedTile){ .len = 2, .index = 49 }; // b1
    if (ptr[1] == 0x32) return (ParsedTile){ .len = 2, .index = 50 }; // b2
    if (ptr[1] == 0x33) return (ParsedTile){ .len = 2, .index = 51 }; // b3
    if (ptr[1] == 0x34) return (ParsedTile){ .len = 2, .index = 52 }; // b4
    if (ptr[1] == 0x35) return (ParsedTile){ .len = 2, .index = 53 }; // b5
    if (ptr[1] == 0x36) return (ParsedTile){ .len = 2, .index = 54 }; // b6
    if (ptr[1] == 0x37) return (ParsedTile){ .len = 2, .index = 55 }; // b7
    if (ptr[1] == 0x38) return (ParsedTile){ .len = 2, .index = 56 }; // b8
    if (ptr[1] == 0x39) return (ParsedTile){ .len = 2, .index = 57 }; // b9
    if (ptr[1] == 0x61) return (ParsedTile){ .len = 2, .index = 58 }; // ba
    if (ptr[1] == 0x62) return (ParsedTile){ .len = 2, .index = 59 }; // bb
    if (ptr[1] == 0x63) return (ParsedTile){ .len = 2, .index = 60 }; // bc
    if (ptr[1] == 0x64) return (ParsedTile){ .len = 2, .index = 61 }; // bd
    if (ptr[1] == 0x65) return (ParsedTile){ .len = 2, .index = 62 }; // be
    if (ptr[1] == 0x66) return (ParsedTile){ .len = 2, .index = 63 }; // bf
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile german_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "Ä", .blank_label = "ä" }, // 2
  { .label = "B", .blank_label = "b" }, // 3
  { .label = "C", .blank_label = "c" }, // 4
  { .label = "D", .blank_label = "d" }, // 5
  { .label = "E", .blank_label = "e" }, // 6
  { .label = "F", .blank_label = "f" }, // 7
  { .label = "G", .blank_label = "g" }, // 8
  { .label = "H", .blank_label = "h" }, // 9
  { .label = "I", .blank_label = "i" }, // 10
  { .label = "J", .blank_label = "j" }, // 11
  { .label = "K", .blank_label = "k" }, // 12
  { .label = "L", .blank_label = "l" }, // 13
  { .label = "M", .blank_label = "m" }, // 14
  { .label = "N", .blank_label = "n" }, // 15
  { .label = "O", .blank_label = "o" }, // 16
  { .label = "Ö", .blank_label = "ö" }, // 17
  { .label = "P", .blank_label = "p" }, // 18
  { .label = "Q", .blank_label = "q" }, // 19
  { .label = "R", .blank_label = "r" }, // 20
  { .label = "S", .blank_label = "s" }, // 21
  { .label = "T", .blank_label = "t" }, // 22
  { .label = "U", .blank_label = "u" }, // 23
  { .label = "Ü", .blank_label = "ü" }, // 24
  { .label = "V", .blank_label = "v" }, // 25
  { .label = "W", .blank_label = "w" }, // 26
  { .label = "X", .blank_label = "x" }, // 27
  { .label = "Y", .blank_label = "y" }, // 28
  { .label = "Z", .blank_label = "z" }, // 29
};

ParsedTile german_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 3 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 4 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 11 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 12 }; // K
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 13 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 16 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 18 }; // P
  case 0x51:
    return (ParsedTile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 21 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 22 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 23 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 25 }; // V
  case 0x57:
    return (ParsedTile){ .len = 1, .index = 26 }; // W
  case 0x58:
    return (ParsedTile){ .len = 1, .index = 27 }; // X
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 28 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 29 }; // Z
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 3 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 4 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 11 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 12 }; // k
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 13 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 16 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 18 }; // p
  case 0x71:
    return (ParsedTile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 21 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 22 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 23 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 25 }; // v
  case 0x77:
    return (ParsedTile){ .len = 1, .index = 26 }; // w
  case 0x78:
    return (ParsedTile){ .len = 1, .index = 27 }; // x
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 28 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 29 }; // z
  case 0xc3:
    if (ptr[1] == 0x84) return (ParsedTile){ .len = 2, .index = 2 }; // Ä
    if (ptr[1] == 0xa4) return (ParsedTile){ .len = 2, .index = 2 }; // ä
    if (ptr[1] == 0x96) return (ParsedTile){ .len = 2, .index = 17 }; // Ö
    if (ptr[1] == 0xb6) return (ParsedTile){ .len = 2, .index = 17 }; // ö
    if (ptr[1] == 0x9c) return (ParsedTile){ .len = 2, .index = 24 }; // Ü
    if (ptr[1] == 0xbc) return (ParsedTile){ .len = 2, .index = 24 }; // ü
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile norwegian_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "B", .blank_label = "b" }, // 2
  { .label = "C", .blank_label = "c" }, // 3
  { .label = "D", .blank_label = "d" }, // 4
  { .label = "E", .blank_label = "e" }, // 5
  { .label = "F", .blank_label = "f" }, // 6
  { .label = "G", .blank_label = "g" }, // 7
  { .label = "H", .blank_label = "h" }, // 8
  { .label = "I", .blank_label = "i" }, // 9
  { .label = "J", .blank_label = "j" }, // 10
  { .label = "K", .blank_label = "k" }, // 11
  { .label = "L", .blank_label = "l" }, // 12
  { .label = "M", .blank_label = "m" }, // 13
  { .label = "N", .blank_label = "n" }, // 14
  { .label = "O", .blank_label = "o" }, // 15
  { .label = "P", .blank_label = "p" }, // 16
  { .label = "Q", .blank_label = "q" }, // 17
  { .label = "R", .blank_label = "r" }, // 18
  { .label = "S", .blank_label = "s" }, // 19
  { .label = "T", .blank_label = "t" }, // 20
  { .label = "U", .blank_label = "u" }, // 21
  { .label = "V", .blank_label = "v" }, // 22
  { .label = "W", .blank_label = "w" }, // 23
  { .label = "X", .blank_label = "x" }, // 24
  { .label = "Y", .blank_label = "y" }, // 25
  { .label = "Ü", .blank_label = "ü" }, // 26
  { .label = "Z", .blank_label = "z" }, // 27
  { .label = "Æ", .blank_label = "æ" }, // 28
  { .label = "Ä", .blank_label = "ä" }, // 29
  { .label = "Ø", .blank_label = "ø" }, // 30
  { .label = "Ö", .blank_label = "ö" }, // 31
  { .label = "Å", .blank_label = "å" }, // 32
};

ParsedTile norwegian_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 4 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 5 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 6 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 7 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 8 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 9 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 10 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 11 }; // K
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 13 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 14 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 15 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 16 }; // P
  case 0x51:
    return (ParsedTile){ .len = 1, .index = 17 }; // Q
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 18 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 19 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 20 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 21 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 22 }; // V
  case 0x57:
    return (ParsedTile){ .len = 1, .index = 23 }; // W
  case 0x58:
    return (ParsedTile){ .len = 1, .index = 24 }; // X
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 25 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 27 }; // Z
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 4 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 5 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 6 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 7 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 8 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 9 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 10 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 11 }; // k
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 13 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 14 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 15 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 16 }; // p
  case 0x71:
    return (ParsedTile){ .len = 1, .index = 17 }; // q
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 18 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 19 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 20 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 21 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 22 }; // v
  case 0x77:
    return (ParsedTile){ .len = 1, .index = 23 }; // w
  case 0x78:
    return (ParsedTile){ .len = 1, .index = 24 }; // x
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 25 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 27 }; // z
  case 0xc3:
    if (ptr[1] == 0x9c) return (ParsedTile){ .len = 2, .index = 26 }; // Ü
    if (ptr[1] == 0xbc) return (ParsedTile){ .len = 2, .index = 26 }; // ü
    if (ptr[1] == 0x86) return (ParsedTile){ .len = 2, .index = 28 }; // Æ
    if (ptr[1] == 0xa6) return (ParsedTile){ .len = 2, .index = 28 }; // æ
    if (ptr[1] == 0x84) return (ParsedTile){ .len = 2, .index = 29 }; // Ä
    if (ptr[1] == 0xa4) return (ParsedTile){ .len = 2, .index = 29 }; // ä
    if (ptr[1] == 0x98) return (ParsedTile){ .len = 2, .index = 30 }; // Ø
    if (ptr[1] == 0xb8) return (ParsedTile){ .len = 2, .index = 30 }; // ø
    if (ptr[1] == 0x96) return (ParsedTile){ .len = 2, .index = 31 }; // Ö
    if (ptr[1] == 0xb6) return (ParsedTile){ .len = 2, .index = 31 }; // ö
    if (ptr[1] == 0x85) return (ParsedTile){ .len = 2, .index = 32 }; // Å
    if (ptr[1] == 0xa5) return (ParsedTile){ .len = 2, .index = 32 }; // å
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile polish_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "Ą", .blank_label = "ą" }, // 2
  { .label = "B", .blank_label = "b" }, // 3
  { .label = "C", .blank_label = "c" }, // 4
  { .label = "Ć", .blank_label = "ć" }, // 5
  { .label = "D", .blank_label = "d" }, // 6
  { .label = "E", .blank_label = "e" }, // 7
  { .label = "Ę", .blank_label = "ę" }, // 8
  { .label = "F", .blank_label = "f" }, // 9
  { .label = "G", .blank_label = "g" }, // 10
  { .label = "H", .blank_label = "h" }, // 11
  { .label = "I", .blank_label = "i" }, // 12
  { .label = "J", .blank_label = "j" }, // 13
  { .label = "K", .blank_label = "k" }, // 14
  { .label = "L", .blank_label = "l" }, // 15
  { .label = "Ł", .blank_label = "ł" }, // 16
  { .label = "M", .blank_label = "m" }, // 17
  { .label = "N", .blank_label = "n" }, // 18
  { .label = "Ń", .blank_label = "ń" }, // 19
  { .label = "O", .blank_label = "o" }, // 20
  { .label = "Ó", .blank_label = "ó" }, // 21
  { .label = "P", .blank_label = "p" }, // 22
  { .label = "R", .blank_label = "r" }, // 23
  { .label = "S", .blank_label = "s" }, // 24
  { .label = "Ś", .blank_label = "ś" }, // 25
  { .label = "T", .blank_label = "t" }, // 26
  { .label = "U", .blank_label = "u" }, // 27
  { .label = "W", .blank_label = "w" }, // 28
  { .label = "Y", .blank_label = "y" }, // 29
  { .label = "Z", .blank_label = "z" }, // 30
  { .label = "Ź", .blank_label = "ź" }, // 31
  { .label = "Ż", .blank_label = "ż" }, // 32
};

ParsedTile polish_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 3 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 4 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 6 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 7 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 9 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 10 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 11 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 12 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 13 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 14 }; // K
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 15 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 17 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 18 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 20 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 22 }; // P
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 23 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 24 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 26 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 27 }; // U
  case 0x57:
    return (ParsedTile){ .len = 1, .index = 28 }; // W
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 29 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 30 }; // Z
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 3 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 4 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 6 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 7 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 9 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 10 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 11 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 12 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 13 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 14 }; // k
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 15 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 17 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 18 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 20 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 22 }; // p
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 23 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 24 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 26 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 27 }; // u
  case 0x77:
    return (ParsedTile){ .len = 1, .index = 28 }; // w
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 29 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 30 }; // z
  case 0xc3:
    if (ptr[1] == 0x93) return (ParsedTile){ .len = 2, .index = 21 }; // Ó
    if (ptr[1] == 0xb3) return (ParsedTile){ .len = 2, .index = 21 }; // ó
    break;
  case 0xc4:
    if (ptr[1] == 0x84) return (ParsedTile){ .len = 2, .index = 2 }; // Ą
    if (ptr[1] == 0x85) return (ParsedTile){ .len = 2, .index = 2 }; // ą
    if (ptr[1] == 0x86) return (ParsedTile){ .len = 2, .index = 5 }; // Ć
    if (ptr[1] == 0x87) return (ParsedTile){ .len = 2, .index = 5 }; // ć
    if (ptr[1] == 0x98) return (ParsedTile){ .len = 2, .index = 8 }; // Ę
    if (ptr[1] == 0x99) return (ParsedTile){ .len = 2, .index = 8 }; // ę
    break;
  case 0xc5:
    if (ptr[1] == 0x81) return (ParsedTile){ .len = 2, .index = 16 }; // Ł
    if (ptr[1] == 0x82) return (ParsedTile){ .len = 2, .index = 16 }; // ł
    if (ptr[1] == 0x83) return (ParsedTile){ .len = 2, .index = 19 }; // Ń
    if (ptr[1] == 0x84) return (ParsedTile){ .len = 2, .index = 19 }; // ń
    if (ptr[1] == 0x9a) return (ParsedTile){ .len = 2, .index = 25 }; // Ś
    if (ptr[1] == 0x9b) return (ParsedTile){ .len = 2, .index = 25 }; // ś
    if (ptr[1] == 0xb9) return (ParsedTile){ .len = 2, .index = 31 }; // Ź
    if (ptr[1] == 0xba) return (ParsedTile){ .len = 2, .index = 31 }; // ź
    if (ptr[1] == 0xbb) return (ParsedTile){ .len = 2, .index = 32 }; // Ż
    if (ptr[1] == 0xbc) return (ParsedTile){ .len = 2, .index = 32 }; // ż
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile slovene_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "B", .blank_label = "b" }, // 2
  { .label = "C", .blank_label = "c" }, // 3
  { .label = "Č", .blank_label = "č" }, // 4
  { .label = "D", .blank_label = "d" }, // 5
  { .label = "E", .blank_label = "e" }, // 6
  { .label = "F", .blank_label = "f" }, // 7
  { .label = "G", .blank_label = "g" }, // 8
  { .label = "H", .blank_label = "h" }, // 9
  { .label = "I", .blank_label = "i" }, // 10
  { .label = "J", .blank_label = "j" }, // 11
  { .label = "K", .blank_label = "k" }, // 12
  { .label = "L", .blank_label = "l" }, // 13
  { .label = "M", .blank_label = "m" }, // 14
  { .label = "N", .blank_label = "n" }, // 15
  { .label = "O", .blank_label = "o" }, // 16
  { .label = "P", .blank_label = "p" }, // 17
  { .label = "R", .blank_label = "r" }, // 18
  { .label = "S", .blank_label = "s" }, // 19
  { .label = "Š", .blank_label = "š" }, // 20
  { .label = "T", .blank_label = "t" }, // 21
  { .label = "U", .blank_label = "u" }, // 22
  { .label = "V", .blank_label = "v" }, // 23
  { .label = "Z", .blank_label = "z" }, // 24
  { .label = "Ž", .blank_label = "ž" }, // 25
};

ParsedTile slovene_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 11 }; // J
  case 0x4b:
    return (ParsedTile){ .len = 1, .index = 12 }; // K
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 13 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 16 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 17 }; // P
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 18 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 19 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 21 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 22 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 23 }; // V
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 24 }; // Z
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 11 }; // j
  case 0x6b:
    return (ParsedTile){ .len = 1, .index = 12 }; // k
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 13 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 16 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 17 }; // p
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 18 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 19 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 21 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 22 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 23 }; // v
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 24 }; // z
  case 0xc4:
    if (ptr[1] == 0x8c) return (ParsedTile){ .len = 2, .index = 4 }; // Č
    if (ptr[1] == 0x8d) return (ParsedTile){ .len = 2, .index = 4 }; // č
    break;
  case 0xc5:
    if (ptr[1] == 0xa0) return (ParsedTile){ .len = 2, .index = 20 }; // Š
    if (ptr[1] == 0xa1) return (ParsedTile){ .len = 2, .index = 20 }; // š
    if (ptr[1] == 0xbd) return (ParsedTile){ .len = 2, .index = 25 }; // Ž
    if (ptr[1] == 0xbe) return (ParsedTile){ .len = 2, .index = 25 }; // ž
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile spanish_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "B", .blank_label = "b" }, // 2
  { .label = "C", .blank_label = "c" }, // 3
  { .label = "[CH]", .blank_label = "[ch]" }, // 4
  { .label = "D", .blank_label = "d" }, // 5
  { .label = "E", .blank_label = "e" }, // 6
  { .label = "F", .blank_label = "f" }, // 7
  { .label = "G", .blank_label = "g" }, // 8
  { .label = "H", .blank_label = "h" }, // 9
  { .label = "I", .blank_label = "i" }, // 10
  { .label = "J", .blank_label = "j" }, // 11
  { .label = "L", .blank_label = "l" }, // 12
  { .label = "[LL]", .blank_label = "[ll]" }, // 13
  { .label = "M", .blank_label = "m" }, // 14
  { .label = "N", .blank_label = "n" }, // 15
  { .label = "Ñ", .blank_label = "ñ" }, // 16
  { .label = "O", .blank_label = "o" }, // 17
  { .label = "P", .blank_label = "p" }, // 18
  { .label = "Q", .blank_label = "q" }, // 19
  { .label = "R", .blank_label = "r" }, // 20
  { .label = "[RR]", .blank_label = "[rr]" }, // 21
  { .label = "S", .blank_label = "s" }, // 22
  { .label = "T", .blank_label = "t" }, // 23
  { .label = "U", .blank_label = "u" }, // 24
  { .label = "V", .blank_label = "v" }, // 25
  { .label = "X", .blank_label = "x" }, // 26
  { .label = "Y", .blank_label = "y" }, // 27
  { .label = "Z", .blank_label = "z" }, // 28
};

ParsedTile spanish_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x31:
    return (ParsedTile){ .len = 1, .index = 4 }; // 1
  case 0x32:
    return (ParsedTile){ .len = 1, .index = 13 }; // 2
  case 0x33:
    return (ParsedTile){ .len = 1, .index = 21 }; // 3
  case 0x3f:
    return (ParsedTile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (ParsedTile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (ParsedTile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (ParsedTile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (ParsedTile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (ParsedTile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (ParsedTile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (ParsedTile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (ParsedTile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (ParsedTile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (ParsedTile){ .len = 1, .index = 11 }; // J
  case 0x4c:
    return (ParsedTile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (ParsedTile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    return (ParsedTile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (ParsedTile){ .len = 1, .index = 17 }; // O
  case 0x50:
    return (ParsedTile){ .len = 1, .index = 18 }; // P
  case 0x51:
    return (ParsedTile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (ParsedTile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (ParsedTile){ .len = 1, .index = 22 }; // S
  case 0x54:
    return (ParsedTile){ .len = 1, .index = 23 }; // T
  case 0x55:
    return (ParsedTile){ .len = 1, .index = 24 }; // U
  case 0x56:
    return (ParsedTile){ .len = 1, .index = 25 }; // V
  case 0x58:
    return (ParsedTile){ .len = 1, .index = 26 }; // X
  case 0x59:
    return (ParsedTile){ .len = 1, .index = 27 }; // Y
  case 0x5a:
    return (ParsedTile){ .len = 1, .index = 28 }; // Z
  case 0x5b:
    if (ptr[1] == 0x43 && ptr[2] == 0x48 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 4 }; // [CH]
    if (ptr[1] == 0x63 && ptr[2] == 0x68 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 4 }; // [ch]
    if (ptr[1] == 0x4c && ptr[2] == 0x4c && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 13 }; // [LL]
    if (ptr[1] == 0x6c && ptr[2] == 0x6c && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 13 }; // [ll]
    if (ptr[1] == 0x52 && ptr[2] == 0x52 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 21 }; // [RR]
    if (ptr[1] == 0x72 && ptr[2] == 0x72 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 21 }; // [rr]
    break;
  case 0x61:
    return (ParsedTile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (ParsedTile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (ParsedTile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (ParsedTile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (ParsedTile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (ParsedTile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (ParsedTile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (ParsedTile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (ParsedTile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (ParsedTile){ .len = 1, .index = 11 }; // j
  case 0x6c:
    return (ParsedTile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (ParsedTile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    return (ParsedTile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (ParsedTile){ .len = 1, .index = 17 }; // o
  case 0x70:
    return (ParsedTile){ .len = 1, .index = 18 }; // p
  case 0x71:
    return (ParsedTile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (ParsedTile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (ParsedTile){ .len = 1, .index = 22 }; // s
  case 0x74:
    return (ParsedTile){ .len = 1, .index = 23 }; // t
  case 0x75:
    return (ParsedTile){ .len = 1, .index = 24 }; // u
  case 0x76:
    return (ParsedTile){ .len = 1, .index = 25 }; // v
  case 0x78:
    return (ParsedTile){ .len = 1, .index = 26 }; // x
  case 0x79:
    return (ParsedTile){ .len = 1, .index = 27 }; // y
  case 0x7a:
    return (ParsedTile){ .len = 1, .index = 28 }; // z
  case 0xc3:
    if (ptr[1] == 0x91) return (ParsedTile){ .len = 2, .index = 16 }; // Ñ
    if (ptr[1] == 0xb1) return (ParsedTile){ .len = 2, .index = 16 }; // ñ
    break;
  }
  return (ParsedTile){ .len = 0, .index = 0 };
}

Tile *super_catalan_tileset = catalan_tileset;
ParsedTile (*super_catalan_tileset_parse)(uint8_t *ptr) = catalan_tileset_parse;

Tile *english_tileset = dutch_tileset;
ParsedTile (*english_tileset_parse)(uint8_t *ptr) = dutch_tileset_parse;

Tile *french_tileset = dutch_tileset;
ParsedTile (*french_tileset_parse)(uint8_t *ptr) = dutch_tileset_parse;

Tile *hong_kong_english_tileset = dutch_tileset;
ParsedTile (*hong_kong_english_tileset_parse)(uint8_t *ptr) = dutch_tileset_parse;

Tile *super_english_tileset = dutch_tileset;
ParsedTile (*super_english_tileset_parse)(uint8_t *ptr) = dutch_tileset_parse;
