#include "pocketstation.h"
#include "lcd.h"

int main() {
  unsigned i;

  lcd_display(0x01234567);
  lcd_display(0x89abcdef);
  lcd_display(0xdeadbeef);
  lcd_display(0xbadc0ffe);
  lcd_display(0xb00000b5);

  /* Busy delay */
  for (i = 0; i < 3000000; i++) {
    nop();
  }
}
