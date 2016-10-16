#include "utils.h"
#include "clock.h"
#include "lcd.h"
#include "pocketstation.h"

int main(void);

/* Linker symbols */
extern uint8_t _sbss[];
extern uint8_t _ebss[];
extern uint8_t _sdata[];
extern uint8_t _edata[];
extern const uint16_t _ldata[];

uint32_t glbl;
uint32_t glbl2 = 42;

/* Entry point */
void _start() {
  uint8_t *p;
  const uint16_t *l;

  /* 0-fill .bss */
  for (p = _sbss; p != _ebss; p++) {
    *p = 0;
  }

  /* Copy .data. We have to be careful because 8bit loads don't work
     with the flash. In particular it means that it's probably a bad
     idea to use a generic "memcpy" here, unless we're sure it never
     uses 8bit reads. */
  for (p = _sdata, l = _ldata;
       p != _edata;
       l++) {
    uint16_t d = *l;

    *p++ = d;

    if (p == _edata) {
      break;
    }

    *p++ = d >> 8;
  }

  /* XXX Should we do something with main's return value? */
  main();

  /* We can't simply return to the GUI, the kernel doesn't setup a
     return address for some reason. Instead we must use system calls
     to "execute" the GUI */
  register uint8_t cur_index __asm__ ("r0");

  __asm__ volatile ("swi 0x16" /* GetDirIndex */
		    : "=r" (cur_index)
		    :
		    : "cc");

  /* Prepare execute: 0x3x to enter the GUI in the file selection
     screen, where "x" should be the index of this program so that it
     starts selected */
  cur_index |= 0x30;

  register uint8_t flag __asm__ ("r0") = 1;
  register uint8_t dir_index __asm__ ("r1") = 0;
  register uint8_t param __asm__ ("r2") = cur_index;

  __asm__ volatile ("swi 0x08" /* PrepareExecute */
		    : "+r"(flag)
		    : "r"(dir_index), "r"(param)
		    : "cc");

  __asm__ volatile ("swi 0x09" /* DoExecute */
		    :
		    :
		    : "r0", "cc");

  /* Should not be reached. Set the clock to the lowest frequency,
     just in case, might as well save some battery. */
  clock_set_frequency(CLOCK_62_5KHZ);

  for (;;) {
  }
}

/* Color icon, 4bits per pixels. Each pixel is an index into the
   icon_palette in _pksx_header_ */
__attribute__ ((section (".header.icon")))
const uint8_t _pksx_icon_[16 * 16 / 2] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x22, 0x22, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x22, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x22, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x00, 0x22, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x22, 0x22, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x22, 0x22, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,
  0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/* Monochrome icon, 1bit per pixel */
__attribute__ ((section (".header.mono_icon")))
const uint32_t _pksx_mono_icon[] = {
  0xffffffff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xfffffcff,
  0xf00000ff,
  0xf00000ff,
  0xffffffff,
};

__attribute__ ((section (".header.mono_icon_descs")))
const struct pksx_mono_icon_desc _pksx_mono_icon_descs_[] = {
  {
    .nframes = ARRAY_SIZE(_pksx_mono_icon) / 32,
    .delay = 1,
    .data = _pksx_mono_icon,
  }
};

__attribute__ ((section (".header.title")))
const struct pksx_executable_header _pksx_header_  = {
  .id = "SC",
  .icon_frame_count = 0x10 | (sizeof(_pksx_icon_) / 128),
  /* XXX can we do better than harcode that? Is this really necessary
     anyway? This info is already encoded in the memory card
     directory... */
  .block_count = 1,
  .title_shift_jis = "Loader",
  .pksx_mono_icons = 0,
  .pksx_id = "MCX0",
  .pksx_exe_icons = ARRAY_SIZE(_pksx_mono_icon_descs_),
  .pksx_functions = 0,
  .pksx_start = _start,
  .icon_palette = { 0x0000, 0x001f, 0x03e0, 0x7c00 },
};
