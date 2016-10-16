#ifndef _IRQ_H_
#define _IRQ_H_

#include "utils.h"

#define IRQ_BUTTON_ACTION    (1U << 0)
#define IRQ_BUTTON_RIGHT     (1U << 1)
#define IRQ_BUTTON_LEFT      (1U << 2)
#define IRQ_BUTTON_DOWN      (1U << 3)
#define IRQ_BUTTON_UP        (1U << 4)
#define FIQ_COM              (1U << 6)
#define IRQ_TIMER_0          (1U << 7)
#define IRQ_TIMER_1          (1U << 8)
#define IRQ_RTC              (1U << 9)
#define IRQ_BATTERY_LOW      (1U << 10)
#define IRQ_DOCKED           (1U << 11)
#define IRQ_IRDA             (1U << 12)
#define FIQ_TIMER_2          (1U << 13)

#define IRQ_ALL              0x1f9fU
#define FIQ_ALL              0x2040U
#define IRQ_FIQ_ALL          (IRQ_ALL | FIQ_ALL)

uint16_t irq_input(void);
uint16_t irq_mask(void);

void irq_set_mask(uint16_t);
void irq_ack(uint16_t);

#endif /* _IRQ_H_ */
