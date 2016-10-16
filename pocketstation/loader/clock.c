#include "clock.h"

#define CLOCK_MODE 0x0b000000

void clock_set_frequency(enum clock_frequency f) {
  write16(CLOCK_MODE, f);

  /* Poll "ready" bit */
  while ((read16(CLOCK_MODE) & 0x10) == 0)
    ;
}
