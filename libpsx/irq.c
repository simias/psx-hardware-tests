#include "irq.h"

#define IRQ_BASE    0x1f801070

#define IRQ_STAT    (IRQ_BASE + 0x00)
#define IRQ_MASK    (IRQ_BASE + 0x04)

uint16_t irq_mask(void) {
  return read16(IRQ_MASK);
}

void irq_set_mask(uint16_t mask) {
  write16(IRQ_MASK, mask);
}
