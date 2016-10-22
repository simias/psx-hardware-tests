#include "irq.h"

#define IRQ_BASE        0x0a000000
#define IRQ_LATCH       (IRQ_BASE + 0x00)
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

/* The volatile is here to prevent some aggressive GCC optimization
   which removes the write to this global for some reason. */
static void (*volatile irq_handler)(void) = NULL;

/* Raw IRQ handler, saving the registers not banked by the IRQ
   switch */
__attribute__ ((__naked__, __target__("arm")))
void irq_wrapper(void) {
  /* Save and restore all the registers that aren't saved in the EABI
     (AAPCS) since GCC will consider that it doesn't have to preserve
     them. Not sure if saving R9 is necessary, but just in case... */
  __asm__ volatile ("push {r0, r1, r2, r3, r9, r12, lr}\n\t"
		    "ldr r0, =irq_handler\n\t"
		    "ldr r0, [r0]\n\t"
		    "cmp r0, #0\n\t"
		    "mov lr, pc\n\t"
		    "bxne r0\n\t"
		    "pop {r0, r1, r2, r3, r9, r12, lr}\n\t"
		    "bx lr");
}


/* Set the raw kernel IRQ callback */
void irq_set_raw_handler(void (*handler)(void)) {
  /* Set IRQ callback */
  register uint32_t index __asm__ ("r0") = 1;
  register uint32_t cb __asm__ ("r1") = (uint32_t)handler;

  __asm__ volatile ("swi 0x01" /* SetCallback */
		    : "+r" (index)
		    : "r" (cb)
		    : "cc");
}

/* Set the kernel IRQ callback, using a wrapper to preserve the
   register values to safely call a regular C function. */
void irq_set_handler(void (*handler)(void)) {
  irq_handler = handler;

  irq_set_raw_handler(irq_wrapper);
}


/* Disable all interrupt handling in the CPU. This has no effect in
   user mode. */
__attribute__ ((__target__("arm")))
void irq_fiq_disable__priviledged(void) {
  register uint32_t tmp;

  __asm__ volatile ("mrs %0, cpsr\n\t"
		    "orr %0, #0xc0\n\t"
		    "msr cpsr_c, %0"
		    : "=r"(tmp));
}

/* Enable all interrupt handling in the CPU. This has no effect in
   user mode. */
__attribute__ ((__target__("arm")))
void irq_fiq_enable__priviledged(void) {
  register uint32_t tmp;

  __asm__ volatile ("mrs %0, cpsr\n\t"
		    "bic %0, #0xc0\n\t"
		    "msr cpsr_c, %0"
		    : "=r"(tmp));
}
