require 'pry-nav'
require 'pathname'

file_path = Pathname.new(File.expand_path('src/rom.c'))

rom = Pathname.new(File.expand_path('ROMs/IBM.ch8'))

rom_file = File.open(rom, 'r')

mapped = rom_file.each_byte.map do |byte|
  "0x#{byte.to_s(16).rjust(2, '0')}"
end
bytes =  mapped.join(', ')


content = <<~CONTENT
  #include <stdint.h>


  uint8_t my_data[] = {
    #{bytes}
  };
CONTENT

File.open(file_path, 'w+') do |file|
  file.puts content
end
