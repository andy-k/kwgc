#!/usr/bin/env ruby
# Copyright (C) 2020-2026 Andy Kurnia.
# throwaway script

def emit_hex_parse(fwd)
  num_tiles = fwd.size
  hi_count = (num_tiles + 15) / 16
  lo_count = [num_tiles, 16].min
  normal_his = (0...hi_count).map {|h| fwd[h * 16][0][0].ord}
  blank_his = (0...hi_count).map {|h| fwd[h * 16][1][0].ord}
  normal_rs = ranges_from(normal_his)
  blank_rs = ranges_from(blank_his)
  puts "  uint8_t hi;"
  first = true
  (normal_rs + blank_rs).each do |lo, hi, offset|
    prefix = first ? "  " : "  else "
    first = false
    if lo == hi
      if offset == 0
        puts "#{prefix}if (ptr[0] == '#{lo}') hi = 0;"
      else
        puts "#{prefix}if (ptr[0] == '#{lo}') hi = ptr[0] - '#{lo}' + #{offset};"
      end
    else
      if offset == 0
        puts "#{prefix}if (ptr[0] >= '#{lo}' && ptr[0] <= '#{hi}') hi = ptr[0] - '#{lo}';"
      else
        puts "#{prefix}if (ptr[0] >= '#{lo}' && ptr[0] <= '#{hi}') hi = ptr[0] - '#{lo}' + #{offset};"
      end
    end
  end
  puts "  else return (ParsedTile){ .len = 0, .index = 0 };"
  lo_chars = (0...lo_count).map {|i| fwd[i][0][1].ord}
  lo_rs = ranges_from(lo_chars)
  puts "  uint8_t lo;"
  first = true
  lo_rs.each do |lo, hi, offset|
    prefix = first ? "  " : "  else "
    first = false
    if offset == 0
      puts "#{prefix}if (ptr[1] >= '#{lo}' && ptr[1] <= '#{hi}') lo = ptr[1] - '#{lo}';"
    else
      puts "#{prefix}if (ptr[1] >= '#{lo}' && ptr[1] <= '#{hi}') lo = ptr[1] - '#{lo}' + #{offset};"
    end
  end
  puts "  else return (ParsedTile){ .len = 0, .index = 0 };"
  puts "  return (ParsedTile){ .len = 2, .index = hi * #{lo_count} + lo };"
end

def ranges_from(chars)
  chars.each_with_index.chunk_while {|(a, ai), (b, bi)| b == a + 1 && bi == ai + 1}
    .map {|grp| [grp.first[0].chr, grp.last[0].chr, grp.first[1]]}
end

def emit_decimal_parse(fwd)
  max_index = fwd.size - 1
  puts "  if (ptr[0] != '[') return (ParsedTile){ .len = 0, .index = 0 };"
  puts "  size_t i = 1;"
  puts "  if (ptr[i] == '-') ++i;"
  puts "  if (ptr[i] < '0' || ptr[i] > '9') return (ParsedTile){ .len = 0, .index = 0 };"
  puts "  int val = ptr[i++] - '0';"
  puts "  if (val > 0 && ptr[i] >= '0' && ptr[i] <= '9') val = val * 10 + ptr[i++] - '0';"
  puts "  if (ptr[i] != ']' || val > #{max_index}) return (ParsedTile){ .len = 0, .index = 0 };"
  puts "  return (ParsedTile){ .len = i + 1, .index = (uint8_t)val };"
end

def emit_map_parse(revmap)
  ascii_map = Array.new(26, 0)
  switch_entries = []
  blank_index = nil

  for fb, len, idx, _, label in revmap
    if label == "?"
      blank_index = idx
    elsif len == 1 && (fb | 0x20) >= 0x61 && (fb | 0x20) <= 0x7a
      ascii_map[(fb | 0x20) - 0x61] = idx
    else
      switch_entries << [fb, label.bytes, idx, label]
    end
  end

  has_invalid = ascii_map.include?(0)
  is_dutch = ascii_map == (1..26).to_a
  has_map = ascii_map.any? {|x| x != 0}

  if has_map && !is_dutch
    puts "  static const uint8_t map[26] = {"
    puts "    #{ascii_map.join(",")}"
    puts "  };"
  end

  puts "  if (*ptr == '?') return (ParsedTile){ .len = 1, .index = #{blank_index} };" if blank_index

  if !switch_entries.empty?
    puts "  switch (*ptr) {"
    for first_byte, entries in switch_entries.group_by {|x| x[0]}.sort_by {|k, _| k}
      puts "  case 0x%02x:" % first_byte
      long_entries, short_entries = entries.partition {|_, b, _, _| b.size > 1}
      for _, bytes, idx, label in long_entries.sort_by {|_, b, i, _| [-b.size, i, b]}
        cond = bytes.each_with_index.drop(1).map {|b, i| "ptr[%d] == 0x%02x" % [i, b]}.join(" && ")
        puts "    if (#{cond}) return (ParsedTile){ .len = #{bytes.size}, .index = #{idx} }; // #{label}"
      end
      if short_entries.empty?
        puts "    break;"
      else
        puts "    return (ParsedTile){ .len = 1, .index = #{short_entries[0][2]} }; // #{short_entries[0][3]}"
      end
    end
    puts "  }"
  end

  if is_dutch
    puts "  uint8_t c = *ptr | 0x20;"
    puts "  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = c - 'a' + 1 };"
  elsif has_map
    puts "  uint8_t c = *ptr | 0x20;"
    if has_invalid
      puts "  if (c >= 'a' && c <= 'z') {"
      puts "    uint8_t idx = map[c - 'a'];"
      puts "    if (idx) return (ParsedTile){ .len = 1, .index = idx };"
      puts "    return (ParsedTile){ .len = 0, .index = 0 };"
      puts "  }"
    else
      puts "  if (c >= 'a' && c <= 'z') return (ParsedTile){ .len = 1, .index = map[c - 'a'] };"
    end
  end

  puts "  return (ParsedTile){ .len = 0, .index = 0 };"
end

main_fn = "../wolges/src/alphabet.rs"
lang = nil
langs = []
mappings = {}
fwd_mappings = {}
lang_src = {}
for x in File.readlines("../wolges/src/alphabet.rs", chomp: true)
  if x =~ /\bfn make_(\w+)_alphabet\b/
    lang = $1
    langs << lang
    mappings[lang] ||= []
    fwd_mappings[lang] ||= []
    lang_src[lang] = nil
  elsif lang && x =~ /\bnew_static_alphabet_from_file!/
    lang_src[lang] = x.scan(/"([^"]*)"/).map(&:first)
  end
end
for lang, fn in lang_src
  idx = -1
  for x in File.readlines(File.join("../wolges/src", fn), chomp: true)
    idx += 1
    toks = x.split
    num_alias_labels = toks[5].to_i
    num_alias_blank_labels = toks[6 + num_alias_labels].to_i
    labels = [*toks[0..1], *toks[6, num_alias_labels], *toks[7 + num_alias_labels, num_alias_blank_labels]]
    fwd_mappings[lang] << labels
    labels.each_with_index do |label, lidx|
      lbytes = label.bytes
      mappings[lang] << [lbytes[0], lbytes.size, idx, lidx, label]
    end
  end
end
revmappings = mappings.inject({}) {|h, (k, v)| h[v] ||= k; h }
# note: french == english
for lang, mapping in mappings
  if lang != revmappings[mapping]
    next
  end
  puts
  puts "Tile #{lang}_tileset[] = {"
  for k, idx in fwd_mappings[lang].each_with_index
    puts "  { .label = #{k[0].inspect}, .blank_label = #{k[1].inspect} }, // #{idx}"
  end
  puts "};"
  seen = {}
  revmap =
    mapping
      .sort_by {|x| [x[0], -x[1], *x[2..]] }
      .select {|_, _, _, _, label| seen[label] ? false : seen[label] = true }
  puts
  puts "ParsedTile #{lang}_tileset_parse(uint8_t *ptr) {"
  if lang == "hex"
    emit_hex_parse(fwd_mappings[lang])
  elsif lang == "decimal"
    emit_decimal_parse(fwd_mappings[lang])
  else
    emit_map_parse(revmap)
  end
  puts "}"
end
for lang, mapping in mappings
  origlang = revmappings[mapping]
  next if lang == origlang
  puts
  puts "Tile *#{lang}_tileset = #{origlang}_tileset;"
  puts "ParsedTile (*#{lang}_tileset_parse)(uint8_t *ptr) = #{origlang}_tileset_parse;"
end
