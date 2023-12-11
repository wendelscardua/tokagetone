#!/usr/bin/ruby
#frozen_string_literal: true

input, output = ARGV

rows = Array.new(64) { [119, 33] * 5 }

def fix_note(note)
  if note =~ /-#/
    note.to_i(16)
  else
    note.gsub(/-/, '').gsub(/#/, 'S')
  end
end

File.readlines(input, chomp: true)
  .each do |row|
    next unless match = row.match(/ROW (?<row_id>..) : (?<sq1note>...) (?<sq1inst>..) . ... : (?<sq2note>...) (?<sq2inst>..) . ... : (?<trinote>...) (?<triinst>..) . ... : (?<noinote>...) (?<noiinst>..) . ... : (?<pcmnote>...) (?<pcminst>..) . .../)
    row_id = match['row_id'].to_i(16)
    if match['sq1note'] != '...'
      rows[row_id][0] = fix_note(match['sq1note'])
      rows[row_id][1] = match['sq1inst'].to_i(16)
    end
    if match['sq2note'] != '...'
      rows[row_id][2] = fix_note(match['sq2note'])
      rows[row_id][3] = match['sq2inst'].to_i(16)
    end
    if match['trinote'] != '...'
      rows[row_id][4] = fix_note(match['trinote'])
      rows[row_id][5] = match['triinst'].to_i(16)
    end
    if match['noinote'] != '...'
      rows[row_id][6] = fix_note(match['noinote'])
      rows[row_id][7] = match['noiinst'].to_i(16)
    end
    if match['pcmnote'] != '...'
      rows[row_id][8] = fix_note(match['pcmnote'])
      rows[row_id][9] = match['pcminst'].to_i(16)
    end
end
File.open(output, 'wb') do |f|
  rows.each do |row|
    f.puts ".byte #{row.join(',')}"
  end
end
