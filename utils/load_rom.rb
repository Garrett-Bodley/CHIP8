require 'pry-nav'
require 'pathname'

if ARGV.empty?
  raise ArgumentError, 'usage: ruby load_rom.rb <pathname>'
end

rom = Pathname.new(File.expand_path(ARGV.shift))

raise ArgumentError, "error: invalid path provided '#{rom}`" unless rom.exist?
raise ArgumentError, "error: not a file '#{rom}`" unless rom.file?
raise ArgumentError, "error: file is not of type .ch8 '#{rom}`" unless rom.extname == '.ch8'

file_path = Pathname.new(File.expand_path('src/apple_rom.c'))

rom_file = File.open(rom, 'rb')

mapped_bytes = rom_file.each_byte.map do |byte|
  "0x#{byte.to_s(16).rjust(2, '0')}"
end
bytes = mapped_bytes.join(', ')

content = <<~CONTENT
  // #{rom.to_s.sub(/.*?(ROMs\/.*)/, '\1')}

  #include <stdint.h>
  #include <stddef.h>

  uint8_t rom[] = {
    #{bytes}
  };

  size_t rom_size = #{mapped_bytes.size};
CONTENT

File.open(file_path, 'w+') do |file|
  file.puts content
end
