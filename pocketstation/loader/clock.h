#ifndef _CLOCK_H_
#define _CLOCK_H_

#include "utils.h"

enum clock_frequency {
  CLOCK_62_5KHZ = 1,
  CLOCK_125KHZ,
  CLOCK_250KHZ,
  CLOCK_500KHZ,
  CLOCK_1MHZ,
  CLOCK_2MHZ,
  CLOCK_4MHZ,
  CLOCK_8MHZ
};

void clock_set_frequency(enum clock_frequency);

enum clock_frequency clock_get_frequency(void);

#endif /* _CLOCK_H_ */
