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

void irq_handler(void) {
  /* Check if the down button is pressed */
  if (irq_input() & IRQ_BUTTON_DOWN) {
    pksx_exit();
  }
}

static void wait_for_docking(void);
static void transfer(void);

int main() {
  /* Mask all interrupts except for the down button (used to force
     exit in irq_handler) */
  irq_set_handler(irq_handler);
  irq_set_mask(IRQ_BUTTON_DOWN);

  /* Acknowledge all interrupts */
  irq_ack(IRQ_FIQ_ALL);

  clock_set_frequency(CLOCK_500KHZ);

  lcd_clear();

  wait_for_docking();

  transfer();

  /* Wait for button interrupt */
  for (;;) {
  }
}


static void wait_for_docking(void) {
  uint32_t delay;

  /* This code is taken from the kernel's docking interrupt. I think
     it also wants to divide the delay by 2 if the clock is locked (or
     something like that) but the code looks bogus so it always ends
     up using this delay. */
  delay = 32 << (int)clock_get_frequency();

  do {
    while ((irq_input() & IRQ_DOCKED) == 0) {
      ;
    }

    /* Once we detect the docking we want to wait a bit in order to
       debounce the signal */
    while (delay--) {
      nop();
    }

    /* Finally if the docking signal is not set after the debounce
       period we ignore it and start again*/
  } while ((irq_input() & IRQ_DOCKED) == 0);
}

static void transfer(void) {
  unsigned i;

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

