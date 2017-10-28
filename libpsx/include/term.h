#ifndef _TERM_H_
#define _TERM_H_

#include "gpu.h"

int term_init(enum gpu_xres xres,
              enum gpu_vmode vmode,
              enum gpu_interlacing interlaced,
              unsigned long backbuffer_lines);

#endif /* _TERM_H_ */
