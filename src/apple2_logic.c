#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MIXED_ON (char *)0xC053
#define MIXED_OFF (char *)0xC052

#define TEXT_ON (char *)0xC051
#define TEXT_OFF (char *)0xC050

#define COLUMN_80_ON (char *)0xC00D
#define COLUMN_80_OFF (char *)0xC00C

// I think maybe these are mislabeled...
#define FULL_SCREEN_COLUMN_80_ON (char *)0xC05E
#define FULL_SCREEN_COLUMN_80_OFF (char *)0xC05F

// Instead maybe call them this?
#define DOUBLE_RES_ON (char *)0xC05E
#define DOUBLE_RES_OFF (char *)0xC05F

#define STORE_80_ON (char *)0xC001
#define STORE_80_OFF (char *)0xC000

#define PAGE_2_ON (char *)0xC055
#define PAGE_2_OFF (char *)0xC054

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
    *PAGE_2_ON = -1;
  }
  else
  {
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

void set_double_res(bool flag)
{
  // Double Resolution Mode
  //
  // The Technical reference manual describes this as DOUBLE HIGH RES
  // It seems tightly coupled to the Enable/Disable state of the IOU (Input Output Unit)
  // I don't fully understand what the Input Output Unit does, or why we need to fiddle with this tbh.

  if(flag)
  {
    *DOUBLE_RES_ON = -1;
  }
  else
  {
    *DOUBLE_RES_OFF = -1;
  }
}

void switch_text_80()
{
  set_mixed(false);
  set_text(true);
  set_store_80(true);
  set_column_80(true);
  set_double_res(true);
  // set_text(true);
}

void set_double_low_res()
{
  set_column_80(true);
  set_double_res(true);
  set_text(false);
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
  char *i = (char *)0x400;
  char *j = (char *)0x428;
  char *k = (char *)0x450;
  for (i; i < (char *)0x7a7; i++)
  {
    *i = 0;
  }
  for (j; j < (char *)0x7cf; j++)
  {
    *j = 0;
  }
  for (k; k < (char *)0x7f8; k++)
  {
    *k = 0;
  }
}