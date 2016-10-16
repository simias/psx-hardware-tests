#include "lcd.h"
#include "irq.h"
#include "clock.h"

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

static void transfer() {
  unsigned i;

  /* Wait for docking signal */
  while ((irq_input() & IRQ_DOCKED) == 0) {
    if (irq_input() & IRQ_BUTTON_ACTION) {
      return;
    }
  }

  lcd_display(irq_input() | 0x10000000);

  /* Write to IOP STOP, stop sound? */
  write32(IOP_STOP, 0x20);

  /* Write to IOP STOP, red LED related? */
  write32(IOP_STOP, 0x02);

  write32(COM_CTRL1, 0);

  /* ACK output low? */
  write32(COM_MODE, 0x02);

  write32(COM_DATA, 0xff);

  write32(COM_CTRL2, 0x03);

  write32(COM_MODE, 0x6);

  write32(COM_CTRL1, 2);

  /* Wait for COM FIQ */
  while ((irq_input() & FIQ_COM) == 0) {
    if (irq_input() & IRQ_BUTTON_ACTION) {
      return;
    }
  }

  lcd_display(irq_input() | 0x20000000);

  write32(COM_CTRL2, 1);

  write32(COM_CTRL1, 3);

  for (;;) {

    uint32_t in;

    /* Poll COM_STAT2 for ready bit */
    while ((read32(COM_STAT2) & 1) == 0) {
      /* Read COM_STAT1, bit 1 should notify an error */
      if (read32(COM_STAT1) & 2) {
	lcd_display(0xdead0);
	return;
      }
    }

    write32(COM_CTRL2, 1);

    /* Read received byte */
    in = read32(COM_DATA);

    lcd_display(in | 0x600D0000);

    if (in != 0x81) {
      return;
    }

    /* Reply */
    write32(COM_DATA, 0x42);

    /* Write ACK? */
    write32(COM_MODE, 1);

    /* XXX what delay here? */
    for (i = 0; i < 10000; i++) {
      nop();
    }

    /* Clear ACK? */
    write32(COM_MODE, 3);

    write32(COM_CTRL2, 1);
  }
}

int main() {
  unsigned i;

  /* Mask all interrupts */
  irq_set_mask(0);

  /* Acknowledge all interrupts */
  irq_ack(IRQ_FIQ_ALL);

  clock_set_frequency(CLOCK_4MHZ);

  while ((irq_input() & IRQ_BUTTON_ACTION) == 1) {
    ;
  }

  lcd_clear();

  while ((irq_input() & IRQ_BUTTON_ACTION) == 0) {
    transfer();
  }

  /* Busy delay */
  for (i = 0; i < 3000000; i++) {
    nop();
  }

  return 0;
}
