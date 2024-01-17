// Copyright (C) 2020-2024 Andy Kurnia.

#include <stdlib.h>

// tile-label-specific

typedef struct {
  char *label;
  char *blank_label;
} tile;

typedef struct {
  size_t len;
  uint8_t index;
} parsed_tile;

tile catalan_tileset[] = {
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

parsed_tile catalan_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 11 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 4 }; // K
  case 0x4c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x4c) return (parsed_tile){ .len = 4, .index = 13 }; // L·L
    return (parsed_tile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    if (ptr[1] == 0x59) return (parsed_tile){ .len = 2, .index = 16 }; // NY
    return (parsed_tile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 17 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 18 }; // P
  case 0x51:
    if (ptr[1] == 0x55) return (parsed_tile){ .len = 2, .index = 19 }; // QU
    return (parsed_tile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 21 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 22 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 23 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 24 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 13 }; // W
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 25 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 16 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 26 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 11 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 4 }; // k
  case 0x6c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x6c) return (parsed_tile){ .len = 4, .index = 13 }; // l·l
    return (parsed_tile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    if (ptr[1] == 0x79) return (parsed_tile){ .len = 2, .index = 16 }; // ny
    return (parsed_tile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 17 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 18 }; // p
  case 0x71:
    if (ptr[1] == 0x75) return (parsed_tile){ .len = 2, .index = 19 }; // qu
    return (parsed_tile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 21 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 22 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 23 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 24 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 13 }; // w
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 25 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 16 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 26 }; // z
  case 0xc3:
    if (ptr[1] == 0x87) return (parsed_tile){ .len = 2, .index = 4 }; // Ç
    if (ptr[1] == 0xa7) return (parsed_tile){ .len = 2, .index = 4 }; // ç
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile english_tileset[] = {
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

parsed_tile english_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 4 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 5 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 6 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 7 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 8 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 9 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 10 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 11 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 13 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 14 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 15 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 16 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 17 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 18 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 19 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 20 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 21 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 22 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 23 }; // W
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 24 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 25 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 26 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 4 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 5 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 6 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 7 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 8 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 9 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 10 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 11 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 13 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 14 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 15 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 16 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 17 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 18 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 19 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 20 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 21 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 22 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 23 }; // w
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 24 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 25 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 26 }; // z
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile german_tileset[] = {
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

parsed_tile german_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 3 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 4 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 11 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 12 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 13 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 16 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 18 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 21 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 22 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 23 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 25 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 26 }; // W
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 27 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 28 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 29 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 3 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 4 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 11 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 12 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 13 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 16 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 18 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 21 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 22 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 23 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 25 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 26 }; // w
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 27 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 28 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 29 }; // z
  case 0xc3:
    if (ptr[1] == 0x84) return (parsed_tile){ .len = 2, .index = 2 }; // Ä
    if (ptr[1] == 0xa4) return (parsed_tile){ .len = 2, .index = 2 }; // ä
    if (ptr[1] == 0x96) return (parsed_tile){ .len = 2, .index = 17 }; // Ö
    if (ptr[1] == 0xb6) return (parsed_tile){ .len = 2, .index = 17 }; // ö
    if (ptr[1] == 0x9c) return (parsed_tile){ .len = 2, .index = 24 }; // Ü
    if (ptr[1] == 0xbc) return (parsed_tile){ .len = 2, .index = 24 }; // ü
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile norwegian_tileset[] = {
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

parsed_tile norwegian_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 4 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 5 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 6 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 7 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 8 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 9 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 10 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 11 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 13 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 14 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 15 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 16 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 17 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 18 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 19 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 20 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 21 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 22 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 23 }; // W
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 24 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 25 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 27 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 4 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 5 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 6 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 7 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 8 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 9 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 10 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 11 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 13 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 14 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 15 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 16 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 17 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 18 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 19 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 20 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 21 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 22 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 23 }; // w
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 24 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 25 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 27 }; // z
  case 0xc3:
    if (ptr[1] == 0x9c) return (parsed_tile){ .len = 2, .index = 26 }; // Ü
    if (ptr[1] == 0xbc) return (parsed_tile){ .len = 2, .index = 26 }; // ü
    if (ptr[1] == 0x86) return (parsed_tile){ .len = 2, .index = 28 }; // Æ
    if (ptr[1] == 0xa6) return (parsed_tile){ .len = 2, .index = 28 }; // æ
    if (ptr[1] == 0x84) return (parsed_tile){ .len = 2, .index = 29 }; // Ä
    if (ptr[1] == 0xa4) return (parsed_tile){ .len = 2, .index = 29 }; // ä
    if (ptr[1] == 0x98) return (parsed_tile){ .len = 2, .index = 30 }; // Ø
    if (ptr[1] == 0xb8) return (parsed_tile){ .len = 2, .index = 30 }; // ø
    if (ptr[1] == 0x96) return (parsed_tile){ .len = 2, .index = 31 }; // Ö
    if (ptr[1] == 0xb6) return (parsed_tile){ .len = 2, .index = 31 }; // ö
    if (ptr[1] == 0x85) return (parsed_tile){ .len = 2, .index = 32 }; // Å
    if (ptr[1] == 0xa5) return (parsed_tile){ .len = 2, .index = 32 }; // å
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile polish_tileset[] = {
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

parsed_tile polish_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 3 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 4 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 6 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 7 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 9 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 10 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 11 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 12 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 13 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 14 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 15 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 17 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 18 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 20 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 22 }; // P
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 23 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 24 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 26 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 27 }; // U
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 28 }; // W
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 29 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 30 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 3 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 4 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 6 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 7 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 9 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 10 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 11 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 12 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 13 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 14 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 15 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 17 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 18 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 20 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 22 }; // p
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 23 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 24 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 26 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 27 }; // u
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 28 }; // w
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 29 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 30 }; // z
  case 0xc3:
    if (ptr[1] == 0x93) return (parsed_tile){ .len = 2, .index = 21 }; // Ó
    if (ptr[1] == 0xb3) return (parsed_tile){ .len = 2, .index = 21 }; // ó
    break;
  case 0xc4:
    if (ptr[1] == 0x84) return (parsed_tile){ .len = 2, .index = 2 }; // Ą
    if (ptr[1] == 0x85) return (parsed_tile){ .len = 2, .index = 2 }; // ą
    if (ptr[1] == 0x86) return (parsed_tile){ .len = 2, .index = 5 }; // Ć
    if (ptr[1] == 0x87) return (parsed_tile){ .len = 2, .index = 5 }; // ć
    if (ptr[1] == 0x98) return (parsed_tile){ .len = 2, .index = 8 }; // Ę
    if (ptr[1] == 0x99) return (parsed_tile){ .len = 2, .index = 8 }; // ę
    break;
  case 0xc5:
    if (ptr[1] == 0x81) return (parsed_tile){ .len = 2, .index = 16 }; // Ł
    if (ptr[1] == 0x82) return (parsed_tile){ .len = 2, .index = 16 }; // ł
    if (ptr[1] == 0x83) return (parsed_tile){ .len = 2, .index = 19 }; // Ń
    if (ptr[1] == 0x84) return (parsed_tile){ .len = 2, .index = 19 }; // ń
    if (ptr[1] == 0x9a) return (parsed_tile){ .len = 2, .index = 25 }; // Ś
    if (ptr[1] == 0x9b) return (parsed_tile){ .len = 2, .index = 25 }; // ś
    if (ptr[1] == 0xb9) return (parsed_tile){ .len = 2, .index = 31 }; // Ź
    if (ptr[1] == 0xba) return (parsed_tile){ .len = 2, .index = 31 }; // ź
    if (ptr[1] == 0xbb) return (parsed_tile){ .len = 2, .index = 32 }; // Ż
    if (ptr[1] == 0xbc) return (parsed_tile){ .len = 2, .index = 32 }; // ż
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile slovene_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "Å", .blank_label = "å" }, // 2
  { .label = "Ä", .blank_label = "ä" }, // 3
  { .label = "B", .blank_label = "b" }, // 4
  { .label = "C", .blank_label = "c" }, // 5
  { .label = "Ç", .blank_label = "ç" }, // 6
  { .label = "Č", .blank_label = "č" }, // 7
  { .label = "D", .blank_label = "d" }, // 8
  { .label = "E", .blank_label = "e" }, // 9
  { .label = "F", .blank_label = "f" }, // 10
  { .label = "G", .blank_label = "g" }, // 11
  { .label = "H", .blank_label = "h" }, // 12
  { .label = "I", .blank_label = "i" }, // 13
  { .label = "J", .blank_label = "j" }, // 14
  { .label = "K", .blank_label = "k" }, // 15
  { .label = "L", .blank_label = "l" }, // 16
  { .label = "M", .blank_label = "m" }, // 17
  { .label = "N", .blank_label = "n" }, // 18
  { .label = "Ñ", .blank_label = "ñ" }, // 19
  { .label = "O", .blank_label = "o" }, // 20
  { .label = "Ö", .blank_label = "ö" }, // 21
  { .label = "P", .blank_label = "p" }, // 22
  { .label = "Q", .blank_label = "q" }, // 23
  { .label = "R", .blank_label = "r" }, // 24
  { .label = "S", .blank_label = "s" }, // 25
  { .label = "Š", .blank_label = "š" }, // 26
  { .label = "T", .blank_label = "t" }, // 27
  { .label = "U", .blank_label = "u" }, // 28
  { .label = "Ü", .blank_label = "ü" }, // 29
  { .label = "V", .blank_label = "v" }, // 30
  { .label = "W", .blank_label = "w" }, // 31
  { .label = "X", .blank_label = "x" }, // 32
  { .label = "Y", .blank_label = "y" }, // 33
  { .label = "Z", .blank_label = "z" }, // 34
  { .label = "Ž", .blank_label = "ž" }, // 35
};

parsed_tile slovene_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 4 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 5 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 8 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 9 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 10 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 11 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 12 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 13 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 14 }; // J
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 15 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 16 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 17 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 18 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 20 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 22 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 23 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 24 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 25 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 27 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 28 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 30 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 31 }; // W
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 32 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 33 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 34 }; // Z
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 4 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 5 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 8 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 9 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 10 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 11 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 12 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 13 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 14 }; // j
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 15 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 16 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 17 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 18 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 20 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 22 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 23 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 24 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 25 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 27 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 28 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 30 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 31 }; // w
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 32 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 33 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 34 }; // z
  case 0xc3:
    if (ptr[1] == 0x85) return (parsed_tile){ .len = 2, .index = 2 }; // Å
    if (ptr[1] == 0xa5) return (parsed_tile){ .len = 2, .index = 2 }; // å
    if (ptr[1] == 0x84) return (parsed_tile){ .len = 2, .index = 3 }; // Ä
    if (ptr[1] == 0xa4) return (parsed_tile){ .len = 2, .index = 3 }; // ä
    if (ptr[1] == 0x87) return (parsed_tile){ .len = 2, .index = 6 }; // Ç
    if (ptr[1] == 0xa7) return (parsed_tile){ .len = 2, .index = 6 }; // ç
    if (ptr[1] == 0x91) return (parsed_tile){ .len = 2, .index = 19 }; // Ñ
    if (ptr[1] == 0xb1) return (parsed_tile){ .len = 2, .index = 19 }; // ñ
    if (ptr[1] == 0x96) return (parsed_tile){ .len = 2, .index = 21 }; // Ö
    if (ptr[1] == 0xb6) return (parsed_tile){ .len = 2, .index = 21 }; // ö
    if (ptr[1] == 0x9c) return (parsed_tile){ .len = 2, .index = 29 }; // Ü
    if (ptr[1] == 0xbc) return (parsed_tile){ .len = 2, .index = 29 }; // ü
    break;
  case 0xc4:
    if (ptr[1] == 0x8c) return (parsed_tile){ .len = 2, .index = 7 }; // Č
    if (ptr[1] == 0x8d) return (parsed_tile){ .len = 2, .index = 7 }; // č
    break;
  case 0xc5:
    if (ptr[1] == 0xa0) return (parsed_tile){ .len = 2, .index = 26 }; // Š
    if (ptr[1] == 0xa1) return (parsed_tile){ .len = 2, .index = 26 }; // š
    if (ptr[1] == 0xbd) return (parsed_tile){ .len = 2, .index = 35 }; // Ž
    if (ptr[1] == 0xbe) return (parsed_tile){ .len = 2, .index = 35 }; // ž
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile spanish_tileset[] = {
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

parsed_tile spanish_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x31:
    return (parsed_tile){ .len = 1, .index = 4 }; // 1
  case 0x32:
    return (parsed_tile){ .len = 1, .index = 13 }; // 2
  case 0x33:
    return (parsed_tile){ .len = 1, .index = 21 }; // 3
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x42:
    return (parsed_tile){ .len = 1, .index = 2 }; // B
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 3 }; // C
  case 0x44:
    return (parsed_tile){ .len = 1, .index = 5 }; // D
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 6 }; // E
  case 0x46:
    return (parsed_tile){ .len = 1, .index = 7 }; // F
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 8 }; // G
  case 0x48:
    return (parsed_tile){ .len = 1, .index = 9 }; // H
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 10 }; // I
  case 0x4a:
    return (parsed_tile){ .len = 1, .index = 11 }; // J
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 12 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 14 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 15 }; // N
  case 0x4f:
    return (parsed_tile){ .len = 1, .index = 17 }; // O
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 18 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 19 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 20 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 22 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 23 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 24 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 25 }; // V
  case 0x58:
    return (parsed_tile){ .len = 1, .index = 26 }; // X
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 27 }; // Y
  case 0x5a:
    return (parsed_tile){ .len = 1, .index = 28 }; // Z
  case 0x5b:
    if (ptr[1] == 0x43 && ptr[2] == 0x48 && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 4 }; // [CH]
    if (ptr[1] == 0x63 && ptr[2] == 0x68 && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 4 }; // [ch]
    if (ptr[1] == 0x4c && ptr[2] == 0x4c && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 13 }; // [LL]
    if (ptr[1] == 0x6c && ptr[2] == 0x6c && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 13 }; // [ll]
    if (ptr[1] == 0x52 && ptr[2] == 0x52 && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 21 }; // [RR]
    if (ptr[1] == 0x72 && ptr[2] == 0x72 && ptr[3] == 0x5d) return (parsed_tile){ .len = 4, .index = 21 }; // [rr]
    break;
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x62:
    return (parsed_tile){ .len = 1, .index = 2 }; // b
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 3 }; // c
  case 0x64:
    return (parsed_tile){ .len = 1, .index = 5 }; // d
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 6 }; // e
  case 0x66:
    return (parsed_tile){ .len = 1, .index = 7 }; // f
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 8 }; // g
  case 0x68:
    return (parsed_tile){ .len = 1, .index = 9 }; // h
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 10 }; // i
  case 0x6a:
    return (parsed_tile){ .len = 1, .index = 11 }; // j
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 12 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 14 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 15 }; // n
  case 0x6f:
    return (parsed_tile){ .len = 1, .index = 17 }; // o
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 18 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 19 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 20 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 22 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 23 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 24 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 25 }; // v
  case 0x78:
    return (parsed_tile){ .len = 1, .index = 26 }; // x
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 27 }; // y
  case 0x7a:
    return (parsed_tile){ .len = 1, .index = 28 }; // z
  case 0xc3:
    if (ptr[1] == 0x91) return (parsed_tile){ .len = 2, .index = 16 }; // Ñ
    if (ptr[1] == 0xb1) return (parsed_tile){ .len = 2, .index = 16 }; // ñ
    break;
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile yupik_tileset[] = {
  { .label = "?", .blank_label = "?" }, // 0
  { .label = "A", .blank_label = "a" }, // 1
  { .label = "C", .blank_label = "c" }, // 2
  { .label = "E", .blank_label = "e" }, // 3
  { .label = "G", .blank_label = "g" }, // 4
  { .label = "I", .blank_label = "i" }, // 5
  { .label = "K", .blank_label = "k" }, // 6
  { .label = "L", .blank_label = "l" }, // 7
  { .label = "M", .blank_label = "m" }, // 8
  { .label = "N", .blank_label = "n" }, // 9
  { .label = "P", .blank_label = "p" }, // 10
  { .label = "Q", .blank_label = "q" }, // 11
  { .label = "R", .blank_label = "r" }, // 12
  { .label = "S", .blank_label = "s" }, // 13
  { .label = "T", .blank_label = "t" }, // 14
  { .label = "U", .blank_label = "u" }, // 15
  { .label = "V", .blank_label = "v" }, // 16
  { .label = "W", .blank_label = "w" }, // 17
  { .label = "Y", .blank_label = "y" }, // 18
};

parsed_tile yupik_tileset_parse(uint8_t *ptr) {
  switch (*ptr) {
  case 0x3f:
    return (parsed_tile){ .len = 1, .index = 0 }; // ?
  case 0x41:
    return (parsed_tile){ .len = 1, .index = 1 }; // A
  case 0x43:
    return (parsed_tile){ .len = 1, .index = 2 }; // C
  case 0x45:
    return (parsed_tile){ .len = 1, .index = 3 }; // E
  case 0x47:
    return (parsed_tile){ .len = 1, .index = 4 }; // G
  case 0x49:
    return (parsed_tile){ .len = 1, .index = 5 }; // I
  case 0x4b:
    return (parsed_tile){ .len = 1, .index = 6 }; // K
  case 0x4c:
    return (parsed_tile){ .len = 1, .index = 7 }; // L
  case 0x4d:
    return (parsed_tile){ .len = 1, .index = 8 }; // M
  case 0x4e:
    return (parsed_tile){ .len = 1, .index = 9 }; // N
  case 0x50:
    return (parsed_tile){ .len = 1, .index = 10 }; // P
  case 0x51:
    return (parsed_tile){ .len = 1, .index = 11 }; // Q
  case 0x52:
    return (parsed_tile){ .len = 1, .index = 12 }; // R
  case 0x53:
    return (parsed_tile){ .len = 1, .index = 13 }; // S
  case 0x54:
    return (parsed_tile){ .len = 1, .index = 14 }; // T
  case 0x55:
    return (parsed_tile){ .len = 1, .index = 15 }; // U
  case 0x56:
    return (parsed_tile){ .len = 1, .index = 16 }; // V
  case 0x57:
    return (parsed_tile){ .len = 1, .index = 17 }; // W
  case 0x59:
    return (parsed_tile){ .len = 1, .index = 18 }; // Y
  case 0x61:
    return (parsed_tile){ .len = 1, .index = 1 }; // a
  case 0x63:
    return (parsed_tile){ .len = 1, .index = 2 }; // c
  case 0x65:
    return (parsed_tile){ .len = 1, .index = 3 }; // e
  case 0x67:
    return (parsed_tile){ .len = 1, .index = 4 }; // g
  case 0x69:
    return (parsed_tile){ .len = 1, .index = 5 }; // i
  case 0x6b:
    return (parsed_tile){ .len = 1, .index = 6 }; // k
  case 0x6c:
    return (parsed_tile){ .len = 1, .index = 7 }; // l
  case 0x6d:
    return (parsed_tile){ .len = 1, .index = 8 }; // m
  case 0x6e:
    return (parsed_tile){ .len = 1, .index = 9 }; // n
  case 0x70:
    return (parsed_tile){ .len = 1, .index = 10 }; // p
  case 0x71:
    return (parsed_tile){ .len = 1, .index = 11 }; // q
  case 0x72:
    return (parsed_tile){ .len = 1, .index = 12 }; // r
  case 0x73:
    return (parsed_tile){ .len = 1, .index = 13 }; // s
  case 0x74:
    return (parsed_tile){ .len = 1, .index = 14 }; // t
  case 0x75:
    return (parsed_tile){ .len = 1, .index = 15 }; // u
  case 0x76:
    return (parsed_tile){ .len = 1, .index = 16 }; // v
  case 0x77:
    return (parsed_tile){ .len = 1, .index = 17 }; // w
  case 0x79:
    return (parsed_tile){ .len = 1, .index = 18 }; // y
  }
  return (parsed_tile){ .len = 0, .index = 0 };
}

tile *super_catalan_tileset = catalan_tileset;
parsed_tile (*super_catalan_tileset_parse)(uint8_t *ptr) = catalan_tileset_parse;

tile *french_tileset = english_tileset;
parsed_tile (*french_tileset_parse)(uint8_t *ptr) = english_tileset_parse;

tile *hong_kong_english_tileset = english_tileset;
parsed_tile (*hong_kong_english_tileset_parse)(uint8_t *ptr) = english_tileset_parse;

tile *super_english_tileset = english_tileset;
parsed_tile (*super_english_tileset_parse)(uint8_t *ptr) = english_tileset_parse;
