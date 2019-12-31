#include "lcd.h"
#include "irq.h"
#include "clock.h"
#include "pocketstation.h"

#define IOP_BASE   0x0d800000
#define IOP_CTRL   (IOP_BASE + 0x00)
#define IOP_STOP   (IOP_BASE + 0x04)

#define COM_BASE   0x0c000000
#define COM_MODE   (COM_BASE + 0x00)
#define COM_STAT1  (COM_BASE + 0x04)
#define COM_DATA   (COM_BASE + 0x08)
#define COM_CTRL1  (COM_BASE + 0x10)
#define COM_STAT2  (COM_BASE + 0x14)
#define COM_CTRL2  (COM_BASE + 0x18)

int main() {
  unsigned i;

  lcd_display(0x600d600d);

  /* Wait for button interrupt */
  for (i = 0; i < 10000; i++) {
    nop();
  }

  pksx_exit();
}

extern struct pksx_callback_ret *func_80(uint32_t flags, uint8_t *data) {
  static uint8_t buf[] = { 0xde, 0xad, 0xbe, 0xef };

  (void)flags;
  (void)data;

  static struct pksx_callback_ret ret =
    {
     .data = buf,
     .data_len = sizeof(buf),
    };

  lcd_display(0xf00ff00f);

  return &ret;
}
