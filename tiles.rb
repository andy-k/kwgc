#!/usr/bin/env ruby
# Copyright (C) 2020-2024 Andy Kurnia.
# throwaway script

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
  puts "  switch (*ptr) {"
  for k, vs in revmap.group_by {|x| x[0] }
    puts "  case 0x%02x:" % k
    vmulti, vsingle = vs.partition {|x| x[1] != 1 }
    for v in vmulti
      puts "    if (#{v[4].bytes.each_with_index.drop(1).map {|b, i| "ptr[%d] == 0x%02x" % [i, b]}.join(" && ")}) return (ParsedTile){ .len = #{v[1]}, .index = #{v[2]} }; // #{v[4]}"
    end
    if vsingle.empty?
      puts "    break;"
    else
      puts "    return (ParsedTile){ .len = 1, .index = #{vsingle[0][2]} }; // #{vsingle[0][4]}"
    end
  end
  puts "  }"
  puts "  return (ParsedTile){ .len = 0, .index = 0 };"
  puts "}"
end
for lang, mapping in mappings
  origlang = revmappings[mapping]
  next if lang == origlang
  puts
  puts "Tile *#{lang}_tileset = #{origlang}_tileset;"
  puts "ParsedTile (*#{lang}_tileset_parse)(uint8_t *ptr) = #{origlang}_tileset_parse;"
end
