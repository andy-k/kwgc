// Copyright (C) 2020-2026 Andy Kurnia.

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
  static const uint8_t map[26] = {
    1,2,3,5,6,7,8,9,10,11,4,12,14,15,17,18,19,20,21,22,23,24,13,25,16,26
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
  case 0x4c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x4c) return (ParsedTile){ .len = 4, .index = 13 }; // L·L
    break;
  case 0x4e:
    if (ptr[1] == 0x59) return (ParsedTile){ .len = 2, .index = 16 }; // NY
    break;
  case 0x51:
    if (ptr[1] == 0x55) return (ParsedTile){ .len = 2, .index = 19 }; // QU
    break;
  case 0x5b:
    if (ptr[1] == 0x4c && ptr[2] == 0xc2 && ptr[3] == 0xb7 && ptr[4] == 0x4c && ptr[5] == 0x5d) return (ParsedTile){ .len = 6, .index = 13 }; // [L·L]
    if (ptr[1] == 0x6c && ptr[2] == 0xc2 && ptr[3] == 0xb7 && ptr[4] == 0x6c && ptr[5] == 0x5d) return (ParsedTile){ .len = 6, .index = 13 }; // [l·l]
    if (ptr[1] == 0x4e && ptr[2] == 0x59 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 16 }; // [NY]
    if (ptr[1] == 0x6e && ptr[2] == 0x79 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 16 }; // [ny]
    if (ptr[1] == 0x51 && ptr[2] == 0x55 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 19 }; // [QU]
    if (ptr[1] == 0x71 && ptr[2] == 0x75 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 19 }; // [qu]
    break;
  case 0x6c:
    if (ptr[1] == 0xc2 && ptr[2] == 0xb7 && ptr[3] == 0x6c) return (ParsedTile){ .len = 4, .index = 13 }; // l·l
    break;
  case 0x6e:
    if (ptr[1] == 0x79) return (ParsedTile){ .len = 2, .index = 16 }; // ny
    break;
  case 0x71:
    if (ptr[1] == 0x75) return (ParsedTile){ .len = 2, .index = 19 }; // qu
    break;
  case 0xc3:
    if (ptr[1] == 0x87) return (ParsedTile){ .len = 2, .index = 4 }; // Ç
    if (ptr[1] == 0xa7) return (ParsedTile){ .len = 2, .index = 4 }; // ç
    break;
  }
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = map[c - 'a'] };
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
  if (ptr[0] != '[') return (ParsedTile){ .len = 0, .index = 0 };
  size_t i = 1;
  if (ptr[i] == '-') ++i;
  if (ptr[i] < '0' || ptr[i] > '9') return (ParsedTile){ .len = 0, .index = 0 };
  int val = ptr[i++] - '0';
  if (val > 0 && ptr[i] >= '0' && ptr[i] <= '9') val = val * 10 + ptr[i++] - '0';
  if (ptr[i] != ']' || val > 63) return (ParsedTile){ .len = 0, .index = 0 };
  return (ParsedTile){ .len = i + 1, .index = (uint8_t)val };
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
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = c - 'a' + 1 };
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
  uint8_t hi;
  if (ptr[0] >= '0' && ptr[0] <= '3') hi = ptr[0] - '0';
  else if (ptr[0] >= '8' && ptr[0] <= '9') hi = ptr[0] - '8';
  else if (ptr[0] >= 'a' && ptr[0] <= 'b') hi = ptr[0] - 'a' + 2;
  else return (ParsedTile){ .len = 0, .index = 0 };
  uint8_t lo;
  if (ptr[1] >= '0' && ptr[1] <= '9') lo = ptr[1] - '0';
  else if (ptr[1] >= 'a' && ptr[1] <= 'f') lo = ptr[1] - 'a' + 10;
  else return (ParsedTile){ .len = 0, .index = 0 };
  return (ParsedTile){ .len = 2, .index = hi * 16 + lo };
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
  static const uint8_t map[26] = {
    1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,18,19,20,21,22,23,25,26,27,28,29
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
  case 0xc3:
    if (ptr[1] == 0x84) return (ParsedTile){ .len = 2, .index = 2 }; // Ä
    if (ptr[1] == 0xa4) return (ParsedTile){ .len = 2, .index = 2 }; // ä
    if (ptr[1] == 0x96) return (ParsedTile){ .len = 2, .index = 17 }; // Ö
    if (ptr[1] == 0xb6) return (ParsedTile){ .len = 2, .index = 17 }; // ö
    if (ptr[1] == 0x9c) return (ParsedTile){ .len = 2, .index = 24 }; // Ü
    if (ptr[1] == 0xbc) return (ParsedTile){ .len = 2, .index = 24 }; // ü
    break;
  }
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = map[c - 'a'] };
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
  static const uint8_t map[26] = {
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,27
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
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
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = map[c - 'a'] };
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
  static const uint8_t map[26] = {
    1,3,4,6,7,9,10,11,12,13,14,15,17,18,20,22,0,23,24,26,27,0,28,0,29,30
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
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
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') {
    uint8_t idx = map[c - 'a'];
    if (idx) return (ParsedTile){ .len = 1, .index = idx };
    return (ParsedTile){ .len = 0, .index = 0 };
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
  { .label = "Q", .blank_label = "q" }, // 18
  { .label = "R", .blank_label = "r" }, // 19
  { .label = "S", .blank_label = "s" }, // 20
  { .label = "Š", .blank_label = "š" }, // 21
  { .label = "T", .blank_label = "t" }, // 22
  { .label = "U", .blank_label = "u" }, // 23
  { .label = "V", .blank_label = "v" }, // 24
  { .label = "W", .blank_label = "w" }, // 25
  { .label = "X", .blank_label = "x" }, // 26
  { .label = "Y", .blank_label = "y" }, // 27
  { .label = "Z", .blank_label = "z" }, // 28
  { .label = "Ž", .blank_label = "ž" }, // 29
};

ParsedTile slovene_tileset_parse(uint8_t *ptr) {
  static const uint8_t map[26] = {
    1,2,3,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,23,24,25,26,27,28
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
  case 0xc4:
    if (ptr[1] == 0x8c) return (ParsedTile){ .len = 2, .index = 4 }; // Č
    if (ptr[1] == 0x8d) return (ParsedTile){ .len = 2, .index = 4 }; // č
    break;
  case 0xc5:
    if (ptr[1] == 0xa0) return (ParsedTile){ .len = 2, .index = 21 }; // Š
    if (ptr[1] == 0xa1) return (ParsedTile){ .len = 2, .index = 21 }; // š
    if (ptr[1] == 0xbd) return (ParsedTile){ .len = 2, .index = 29 }; // Ž
    if (ptr[1] == 0xbe) return (ParsedTile){ .len = 2, .index = 29 }; // ž
    break;
  }
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = map[c - 'a'] };
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
  static const uint8_t map[26] = {
    1,2,3,5,6,7,8,9,10,11,0,12,14,15,17,18,19,20,22,23,24,25,0,26,27,28
  };
  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = 0 };
  switch (*ptr) {
  case 0x31:
    return (ParsedTile){ .len = 1, .index = 4 }; // 1
  case 0x32:
    return (ParsedTile){ .len = 1, .index = 13 }; // 2
  case 0x33:
    return (ParsedTile){ .len = 1, .index = 21 }; // 3
  case 0x5b:
    if (ptr[1] == 0x43 && ptr[2] == 0x48 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 4 }; // [CH]
    if (ptr[1] == 0x63 && ptr[2] == 0x68 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 4 }; // [ch]
    if (ptr[1] == 0x4c && ptr[2] == 0x4c && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 13 }; // [LL]
    if (ptr[1] == 0x6c && ptr[2] == 0x6c && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 13 }; // [ll]
    if (ptr[1] == 0x52 && ptr[2] == 0x52 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 21 }; // [RR]
    if (ptr[1] == 0x72 && ptr[2] == 0x72 && ptr[3] == 0x5d) return (ParsedTile){ .len = 4, .index = 21 }; // [rr]
    break;
  case 0xc3:
    if (ptr[1] == 0x91) return (ParsedTile){ .len = 2, .index = 16 }; // Ñ
    if (ptr[1] == 0xb1) return (ParsedTile){ .len = 2, .index = 16 }; // ñ
    break;
  }
  uint8_t c = *ptr | 0x20;
  if (c >= 'a' && c <= 'z') {
    uint8_t idx = map[c - 'a'];
    if (idx) return (ParsedTile){ .len = 1, .index = idx };
    return (ParsedTile){ .len = 0, .index = 0 };
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
