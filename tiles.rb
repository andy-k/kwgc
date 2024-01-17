#!/usr/bin/env ruby
# Copyright (C) 2020-2024 Andy Kurnia.
# throwaway script

lang = nil
langs = []
mappings = {}
fwd_mappings = {}
for x in File.readlines("../wolges/src/alphabet.rs", chomp: true)
  if x =~ /\bfn make_(\w+)_alphabet\b/
    lang = $1
    langs << lang
    mappings[lang] ||= []
    fwd_mappings[lang] ||= []
    idx = -1
  elsif lang && x =~ /\btile!/
    idx += 1
    labels = x.scan(/"([^"]*)"/).map(&:first)
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
  puts "tile #{lang}_tileset[] = {"
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
  puts "parsed_tile #{lang}_tileset_parse(uint8_t *ptr) {"
  puts "  switch (*ptr) {"
  for k, vs in revmap.group_by {|x| x[0] }
    puts "  case 0x%02x:" % k
    vmulti, vsingle = vs.partition {|x| x[1] != 1 }
    for v in vmulti
      puts "    if (#{v[4].bytes.each_with_index.drop(1).map {|b, i| "ptr[%d] == 0x%02x" % [i, b]}.join(" && ")}) return (parsed_tile){ .len = #{v[1]}, .index = #{v[2]} }; // #{v[4]}"
    end
    if vsingle.empty?
      puts "    break;"
    else
      puts "    return (parsed_tile){ .len = 1, .index = #{vsingle[0][2]} }; // #{vsingle[0][4]}"
    end
  end
  puts "  }"
  puts "  return (parsed_tile){ .len = 0, .index = 0 };"
  puts "}"
end
for lang, mapping in mappings
  origlang = revmappings[mapping]
  next if lang == origlang
  puts
  puts "tile *#{lang}_tileset = #{origlang}_tileset;"
  puts "parsed_tile (*#{lang}_tileset_parse)(uint8_t *ptr) = #{origlang}_tileset_parse;"
end
