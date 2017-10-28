#ifndef _BIOS_H_
#define _BIOS_H_

#include "utils.h"

void bios_printf(const char *, ...)
  __attribute__ ((format (printf, 1, 2)));

void bios_gpu_sync(void);

void *bios_malloc(size_t);
void bios_free(void *);

func_ptr_t *bios_get_b0_table(void);

#endif /* _BIOS_H_ */
