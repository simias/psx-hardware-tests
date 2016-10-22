#include "clock.h"

#define CLOCK_MODE 0x0b000000

void clock_set_frequency(enum clock_frequency f) {
  write16(CLOCK_MODE, f);

  /* Poll "ready" bit */
  while ((read16(CLOCK_MODE) & 0x10) == 0)
    ;
}

enum clock_frequency clock_get_frequency(void) {
  uint16_t f = read16(CLOCK_MODE) & 0xf;

  /* XXX double check this behaviour. I think all values above 8 do
     the same */
  if (f > 8) {
    f = 8;
  }

  return f;
}
