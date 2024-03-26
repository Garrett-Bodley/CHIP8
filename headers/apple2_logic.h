#ifndef APPLE2_LOGIC_H
#define APPLE2_LOGIC_H
#include <stdbool.h>

void set_mixed(bool flag);
void set_text(bool flag);
void set_column_80(bool flag);
void set_fs_c80(bool flag);
void switch_lgd_80(bool flag);
void switch_text_80(bool flag);

void fill_lgd();
void set_double_low_res();

void apple_sleep(uint8_t seconds);
void fill_lgd();
void clear_lgd();

void set_page_2(bool flag);
void set_double_res(bool flag);

#endif