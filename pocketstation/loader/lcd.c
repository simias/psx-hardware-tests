#include "lcd.h"

/* Start of the LCD memory area */
#define LCD_BASE            0x0d000000U
/* Offset of the first byte of the first line of the framebuffer. The
 * screen is 32x32 pixels, each pixel is mapped to a single bit in
 * the framebuffer. */
#define LCD_FRAMEBUFFER     (LCD_BASE + 0x100)

void lcd_write_line(uint8_t pos, uint32_t v) {
  pos &= 31;

  write32(LCD_FRAMEBUFFER + pos * 4, v);
}

uint32_t lcd_read_line(uint8_t pos) {
  pos &= 31;

  return read32(LCD_FRAMEBUFFER + pos * 4);
}

/* Clear the entire LCD */
void lcd_clear(void) {
  int i;

  for (i = 0; i < 32; i++) {
    lcd_write_line(i, 0);
  }
}

/* Scroll the display upwards by `lines` pixels */
void lcd_scroll(uint8_t lines) {
  if (lines >= 32) {
    lcd_clear();
  } else {
    unsigned i;

    for (i = lines; i < 32; i++) {
      uint32_t l = lcd_read_line(i);

      lcd_write_line(i, 0);
      lcd_write_line(i - lines, l);
    }
  }
}

/* Hexadecimal character font. Each character is 4x5 pixels.
 *
 * In order to save a bit of space I pack two characters in 5
 * bytes. For instance the character '2' is in the low nibbles of
 * hex_font[2][0..5] while '3' is in the high nibbles of the same
 * entries. */
static const uint8_t hex_font[8][5] = {
  { 0x4e, 0x6a, 0x4a, 0x4a, 0xee },
  { 0xee, 0x88, 0xce, 0x82, 0xee },
  { 0xe8, 0x2c, 0xea, 0x8e, 0xe8 },
  { 0xee, 0x82, 0x4e, 0x4a, 0x4e },
  { 0xee, 0xaa, 0xee, 0x8a, 0xee },
  { 0x64, 0xaa, 0x6e, 0xaa, 0x6a },
  { 0x6c, 0xa2, 0xa2, 0xa2, 0x6c },
  { 0xee, 0x22, 0x66, 0x22, 0x2e },
};

/* Scroll the LCD and display a single u32 in a human-readable way */
void lcd_display(uint32_t val) {
  int line, byte;

  lcd_scroll(6);

  for (line = 0; line < 5; line++) {

    uint32_t pixels = 0;

    for (byte = 0; byte < 8; byte++) {
      unsigned nibble = (val >> (byte * 4)) & 0xf;

      uint32_t c = hex_font[nibble / 2][line];

      c = (c >> ((nibble & 1) * 4)) & 0xf;

      pixels |= c << ((7 - byte) * 4);
    }

    lcd_write_line(26 + line, pixels);
  }
}
