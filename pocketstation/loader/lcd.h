#ifndef _LCD_H_
#define _LCD_H_

#include "utils.h"

void lcd_clear(void);
void lcd_scroll(uint8_t);
void lcd_write_line(uint8_t, uint32_t);
uint32_t lcd_read_line(uint8_t);
void lcd_display(uint32_t);
void lcd_rotate(bool);

#endif
