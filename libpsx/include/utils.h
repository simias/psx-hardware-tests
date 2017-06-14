#ifndef _UTILS_H_
#define _UTILS_H_

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef _Bool           bool;
typedef unsigned int    size_t;

#define ARRAY_SIZE(_a) (sizeof(_a) / sizeof(*(_a)))

#define NULL ((void*)0)

typedef _Bool bool;

#define true  1
#define false 0

static inline void nop(void) {
  __asm__ __volatile__ ("nop");
}

static inline void brk(void) {
  __asm__ __volatile__ ("break");
}

static inline uint32_t read32(size_t addr) {
  volatile const uint32_t *p = (volatile void *)addr;

  return *p;
}

static inline uint16_t read16(size_t addr) {
  volatile const uint16_t *p = (volatile void *)addr;

  return *p;
}

static inline uint8_t read8(size_t addr) {
  volatile const uint8_t *p = (volatile void *)addr;

  return *p;
}

static inline void write32(size_t addr, uint32_t val) {
  volatile uint32_t *p = (volatile void *)addr;

  *p = val;
}

static inline void write16(size_t addr, uint16_t val) {
  volatile uint16_t *p = (volatile void *)addr;

  *p = val;
}

static inline void write8(size_t addr, uint8_t val) {
  volatile uint8_t *p = (volatile void *)addr;

  *p = val;
}

#endif /* _UTILS_H_ */
