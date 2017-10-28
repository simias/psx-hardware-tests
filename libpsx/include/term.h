#ifndef _TERM_H_
#define _TERM_H_

#include "gpu.h"

int term_init(enum gpu_xres xres,
              enum gpu_vmode vmode,
              enum gpu_interlacing interlaced,
              unsigned long backbuffer_lines);

void term_close(void);

void term_putchar(int c);

void term_hook_bios_puchar(void);
void term_unhook_bios_puchar(void);

#endif /* _TERM_H_ */
