#include <stdbool.h>
#include <stdint.h>
#ifndef APPLE2_LOGIC_H
#define APPLE2_LOGIC_H

#define FIRST_SCREEN_THIRD 0x400
#define SECOND_SCREEN_THIRD 0x428
#define LAST_SCREEN_THIRD 0x450
#define SCREEN_ROW_OFFSET 0x80

#define SPEAKER_TOGGLE (volatile uint8_t*) 0xC030 // Read from this address to drive the speaker once

#define KEY_DATA ((volatile uint8_t*) 0xC000) // Contains char code of current keypress and flag bit
#define CLEAR_KEY_STROBE ((volatile uint8_t*) 0xC010) // Peek or poke to reset flag bit

void set_mixed(bool flag);
void set_text(bool flag);
void set_column_80(bool flag);
void set_fs_c80(bool flag);
void switch_lgd_80(void);
void switch_text_80(void);

void fill_lgd();
void set_double_low_res();

void apple_sleep(uint8_t seconds);
void fill_lgd();
void clear_lgd();

void set_page_2(bool flag);
void set_double_hires(bool flag);

void draw_lg80(uint8_t x, uint8_t y);
void clear_40_col_text();
void switch_text_40();
void draw_sprite_word_lg80(uint8_t x, uint8_t y, uint8_t sprite_word);
void draw_two_sprite_words_lgd80(uint8_t x, uint8_t y, uint8_t sprite_word1, uint8_t sprite_word2);
void draw_sprite_words_lg80(uint8_t x, uint8_t y, uint8_t* sprite, uint8_t n);
char read_char(void);
char char_to_hex(char ch);

#endif