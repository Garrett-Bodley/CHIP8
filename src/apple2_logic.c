#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MIXED_ON (uint8_t *)0xC053
#define MIXED_OFF (uint8_t *)0xC052

#define TEXT_ON (uint8_t *)0xC051
#define TEXT_OFF (uint8_t *)0xC050

#define COLUMN_80_ON (uint8_t *)0xC00D
#define COLUMN_80_OFF (uint8_t *)0xC00C

// I think maybe these are mislabeled...
#define FULL_SCREEN_COLUMN_80_ON (uint8_t *)0xC05E
#define FULL_SCREEN_COLUMN_80_OFF (uint8_t *)0xC05F

// Instead maybe call them this?
#define DOUBLE_HIRES_ON (uint8_t *)0xC05E
#define DOUBLE_HIRES_OFF (uint8_t *)0xC05F

#define STORE_80_ON (uint8_t *)0xC001
#define STORE_80_OFF (uint8_t *)0xC000

#define PAGE_2_ON (uint8_t *)0xC055
#define PAGE_2_OFF (uint8_t *)0xC054

void apple_sleep(uint8_t seconds){
    volatile uint16_t i;
    for (; seconds > 0; seconds--) {
        for (i = 0; i < 30000; i++) {
            // Empty loop for delay
        }
    }
}

void set_mixed(bool flag)
{
  if (flag)
  {
    *MIXED_ON = -1;
  }
  else
  {
    *MIXED_OFF = -1;
  }
}

void set_text(bool flag)
{
  if (flag)
  {
    *TEXT_ON = -1;
  }
  else
  {
    *TEXT_OFF = -1;
  }
}

void set_column_80(bool flag)
{
  if (flag)
  {
    *COLUMN_80_ON = -1;
  }
  else
  {
    *COLUMN_80_OFF = -1;
  }
}

// I think maybe this function should be deprecated
void set_fs_c80(bool flag)
{
  if (flag)
  {
    *FULL_SCREEN_COLUMN_80_ON = -1;
  }
  else
  {
    *FULL_SCREEN_COLUMN_80_OFF = -1;
  }
}

void switch_lgd_80()
{
  set_mixed(false);
  set_text(false);
  set_column_80(true);
}

void set_page_2(bool flag)
{
  if (flag)
  {
    // 0xC055 - On: select Page 2 or, if 80 STORE on, Page 1 in auxiliary memory
    *PAGE_2_ON = -1;
  }
  else
  {
    // 0xC054 — Off: select Page 1
    *PAGE_2_OFF = -1;
  }
}

void set_store_80(bool flag)
{
  // The 80 STORE soft switch determines what block of memory "page 2" points to
  //
  // If 80 STORE is ON, "page 2" refers to aux memory, e.g. the interleaved memory used for 80-column mode
  // If 80 STORE is OFF, "page 2" refers to the memory space 0x800-0xBFF
  //
  // This is described on page 27 of the Apple //e technical reference manual
  // https://archive.org/details/Apple_IIe_Technical_Reference_Manual/page/n59/mode/2up?q=double+low+res
  if(flag)
  {
    *STORE_80_ON = -1;
  }else
  {
    *STORE_80_OFF = -1;
  }
}

void set_double_hires(bool flag)
{
  // Double Resolution Mode
  //
  // The Technical reference manual describes this as DOUBLE HIGH RES
  // It seems tightly coupled to the Enable/Disable state of the IOU (Input Output Unit)
  // I don't fully understand what the Input Output Unit does, or why we need to fiddle with this tbh.

  // IOUDIS is on at boot time with the Virtual II
  // Unsure if this is true on the physical Apple //e at the Hub
  if(flag)
  {
    // On: if IOUDIS on, turn on double-high-res.
    *DOUBLE_HIRES_ON = -1;
  }
  else
  {
    // Off: if IOUDIS on, turn off double-high-res
    *DOUBLE_HIRES_OFF = -1;
  }
}

void switch_text_80()
{
  set_mixed(false);
  set_text(true);
  set_store_80(false);
  set_column_80(true);
  set_double_hires(true);
  // set_text(true);
}

void switch_text_40()
{
  // This doesn't work. I get a weird jumble of characters and an error message
  // on the screen when I try to switch back to 40 col text mode
  // Kind of looks like garbage values tho. Maybe there's a system routine that's getting jumbled by my code?
  set_mixed(false);
  set_text(true);
  set_store_80(false);
  set_column_80(false);
  set_double_hires(false);
}

void set_double_low_res()
{
  set_text(false);
  set_column_80(true);
  set_store_80(true);
  set_double_hires(true);
}

void fill_lgd()
{
  char *i = (char *)0x400;
  char *j = (char *)0x428;
  char *k = (char *)0x450;
  for (i; i < (char *)0x7a7; i++)
  {
    *i = 0xFF;
  }
  for (j; j < (char *)0x7cf; j++)
  {
    *j = 0xFF;
  }
  for (k; k < (char *)0x7f8; k++)
  {
    *k = 0xFF;
  }
}

void clear_lgd()
{
  char *one = (char *)0x400;
  char *two = (char *)0x480;
  char *three = (char *)0x500;
  char *four = (char *)0x580;
  char *five = (char *)0x600;
  char *six = (char *)0x680;
  char *seven = (char *)0x700;
  char *eight = (char *)0x780;

  memset(one, 0x00, 120);
  memset(two, 0x00, 120);
  memset(three, 0x00, 120);
  memset(four, 0x00, 120);
  memset(five, 0x00, 120);
  memset(six, 0x00, 120);
  memset(seven, 0x00, 120);
  memset(eight, 0x00, 120);
}

void clear_40_col_text()
{
  char *one = (char *)0x400;
  char *two = (char *)0x480;
  char *three = (char *)0x500;
  char *four = (char *)0x580;
  char *five = (char *)0x600;
  char *six = (char *)0x680;
  char *seven = (char *)0x700;
  char *eight = (char *)0x780;

  memset(one, ' ' | 0x80, 120);
  memset(two, ' ' | 0x80, 120);
  memset(three, ' ' | 0x80, 120);
  memset(four, ' ' | 0x80, 120);
  memset(five, ' ' | 0x80, 120);
  memset(six, ' ' | 0x80, 120);
  memset(seven, ' ' | 0x80, 120);
  memset(eight, ' ' | 0x80, 120);
}

void draw_lg80(uint8_t x, uint8_t y){
  uint8_t x_mem_offset, val_to_write;
  uint16_t y_mem_base;
  bool screen_nibble_high, page2_flag;
  uint8_t * address_to_write;

  if( (y & 1) == 0)
  {
    screen_nibble_high = false;
  }
  else
  {
    screen_nibble_high = true;
  }

  if(screen_nibble_high){
    val_to_write = 0xF0;
  }else{
    val_to_write = 0xF;
  }

  if((x & 1) == 0){
    page2_flag = true;
  }
  else
  {
    page2_flag = false;
  }
  set_page_2(page2_flag);

  x_mem_offset = x >> 1;
  if((y >> 1) < 8)
  {
    y_mem_base = 0x400;
  }
  else if((y >> 1) < 16)
  {
    y_mem_base = 0x4A8;
  }
  else
  {
    y_mem_base = 0x450;
  }
  y_mem_base += (((y >> 1) & 7) * 0x80);
  address_to_write = (uint8_t*)(y_mem_base + x_mem_offset);
  *address_to_write ^= val_to_write;
}

void draw_sprite_word_lg80(uint8_t x, uint8_t y, uint8_t sprite_word)
{
    uint8_t i, j, bit_to_write, val_to_write, x_mem_offset;
    uint16_t y_mem_base;
    uint8_t * address_to_write;
    bool screen_nibble_high;
    bool page2_flag;

    uint8_t sprite_mask = 128;

    // Pad for //e screen
    x += 8;
    y += 8;

    if ((y & 1) == 0)
    {
      screen_nibble_high = false;
    }else{
      screen_nibble_high = true;
    }

    if((x & 1) == 0){
      page2_flag = true;
    }else{
      page2_flag = false;
    }

  for(j = 0; j < 8; j++)
  {
    // each j is a column

    // We don't have to do anything if the bit we're looking at is empty
    bit_to_write = sprite_word & sprite_mask;
    if(bit_to_write == 0) {
      sprite_mask >>= 1;
      page2_flag = !page2_flag;
      ++x;
      continue;
    }

    // Set soft switch to write to the correct memory page
    set_page_2(page2_flag);
    if(screen_nibble_high){
        val_to_write = 0xF0;
    }else{
      val_to_write = 0xF;
    }

    x_mem_offset = x / 2;

    if((y >> 1) < 8)
    {
      y_mem_base = 0x400;
    }
    else if((y >> 1) < 16)
    {
      y_mem_base = 0x4A8;
    }
    else
    {
      y_mem_base = 0x450;
    }
    y_mem_base += (((y >> 1) & 7) * 0x80);

    address_to_write = (uint8_t*)(y_mem_base + x_mem_offset);
    *address_to_write ^= val_to_write;

    // change which page i'm writing to based on the value of J
    sprite_mask >>= 1;
    page2_flag = !page2_flag;
    ++x;
  }
  screen_nibble_high = !screen_nibble_high;
}

void draw_sprite_words_lg80(uint8_t x, uint8_t y, uint8_t* sprite, uint8_t n)
{
  uint8_t j, bit_to_write, sprite_word, val_to_write, x_mem_offset;
  uint16_t i, y_mem_base;
  uint8_t * address_to_write;
  bool screen_nibble_high;
  bool page2_flag;

  uint8_t sprite_mask = 128;

  // Pad for //e screen
  x += 8;
  y += 8;
  if ((y & 1) == 0)
  {
    screen_nibble_high = false;
  }else{
    screen_nibble_high = true;
  }

  if((x & 1) == 0){
    page2_flag = true;
  }else{
    page2_flag = false;
  }

  for(i = 0; i < n; i++){
    if(i == 1) draw_lg80(40, 0);
    sprite_word = (uint8_t)sprite[i];
    for(j = 0; j < 8; j++)
    {
      // each j is a column
      // We don't have to do anything if the bit we're looking at is empty
      bit_to_write = sprite_word & sprite_mask;
      if(bit_to_write == 0) {
        sprite_mask >>= 1;
        page2_flag = !page2_flag;
        ++x;
        continue;
      }

      // Set soft switch to write to the correct memory page
      set_page_2(page2_flag);

      if((screen_nibble_high)){
        val_to_write = 0xF0;
      }else{
        val_to_write = 0x0F;
      }

      x_mem_offset = x / 2;

      if((y >> 1) < 8)
      {
        y_mem_base = 0x400;
      }
      else if((y >> 1) < 16)
      {
        y_mem_base = 0x4A8;
      }
      else
      {
        y_mem_base = 0x450;
      }
      y_mem_base += (((y >> 1) & 7) * 0x80);

      address_to_write = (uint8_t*)(y_mem_base + x_mem_offset);
      *address_to_write ^= val_to_write;

      // change which page i'm writing to based on the value of J
      sprite_mask >>= 1;
      page2_flag = !page2_flag;
      ++x;
    }
    screen_nibble_high = !screen_nibble_high;
    // screen_nibble_high = false;
    y++;
  }
}