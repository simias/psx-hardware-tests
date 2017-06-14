#ifndef _IRQ_H_
#define _IRQ_H_

#include "utils.h"

#define IRQ_VBLANK    (1U << 0)
#define IRQ_GPU       (1U << 1)
#define IRQ_CDROM     (1U << 2)
#define IRQ_DMA       (1U << 3)
#define IRQ_TIMER_0   (1U << 4)
#define IRQ_TIMER_1   (1U << 5)
#define IRQ_TIMER_2   (1U << 6)
#define IRQ_PAD_MC    (1U << 7)
#define IRQ_SIO       (1U << 8)
#define IRQ_SPU       (1U << 9)

uint16_t irq_mask(void);
void irq_set_mask(uint16_t);

#endif /* _IRQ_H_ */
