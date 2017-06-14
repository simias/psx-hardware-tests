#ifndef _BIOS_H_
#define _BIOS_H_

void bios_printf(const char *, ...)
  __attribute__ ((format (printf, 1, 2)));
void bios_gpu_sync(void);

#endif /* _BIOS_H_ */
