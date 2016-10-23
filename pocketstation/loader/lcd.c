#include "lcd.h"

/* Start of the LCD memory area */
#define LCD_BASE            0x0d000000U
#define LCD_MODE            (LCD_BASE + 0x00)
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
 * 8 characters are packed in each row, one in each nibble of the
 * array.
 *
 * You can't make this array an u8 because FLASH doesn't support 8bit
 * access. I found that out the hard way... */
static const uint32_t hex_font[2][5] = {
  { 0xeee8ee4e, 0x822c886a, 0x4eeace4a, 0x4a8e824a, 0x4ee8eeee },
  { 0xee6c64ee, 0x22a2aaaa, 0x66a26eee, 0x22a2aa8a, 0x2e6c6aee },
};

/* Scroll the LCD and display a single u32 in a human-readable way */
void lcd_display(uint32_t val) {
  int line, byte;

  lcd_scroll(6);

  for (line = 0; line < 5; line++) {

    uint32_t pixels = 0;

    for (byte = 0; byte < 8; byte++) {
      unsigned nibble = (val >> (byte * 4)) & 0xf;

      uint32_t c = hex_font[nibble / 8][line];

      c = (c >> ((nibble & 7) * 4)) & 0xf;

      pixels |= c << ((7 - byte) * 4);
    }

    lcd_write_line(26 + line, pixels);
  }
}

void lcd_rotate(bool rotate) {
  uint32_t mode = read32(LCD_MODE);

  mode &= ~0x80;
  mode |= (uint32_t)rotate << 7;;

  write32(LCD_MODE, mode);
}
