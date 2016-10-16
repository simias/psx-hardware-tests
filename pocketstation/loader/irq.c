#include "irq.h"

#define IRQ_BASE        0x0a000000
#define IRQ_INPUT       (IRQ_BASE + 0x04)
#define IRQ_MASK_SET    (IRQ_BASE + 0x08)
#define IRQ_MASK_READ   (IRQ_BASE + 0x08)
#define IRQ_MASK_CLR    (IRQ_BASE + 0x0c)
#define IRQ_ACK         (IRQ_BASE + 0x10)

uint16_t irq_input(void) {
  return read16(IRQ_INPUT);
}

uint16_t irq_mask(void) {
  return read16(IRQ_MASK_READ);
}

void irq_set_mask(uint16_t mask) {
  write16(IRQ_MASK_SET, mask);
  write16(IRQ_MASK_CLR, ~mask);
}

void irq_ack(uint16_t ack) {
  write16(IRQ_ACK, ack);
}
