#ifndef _POCKETSTATION_H_
#define _POCKETSTATION_H_

#include "utils.h"

/* Pocketstation executable header, located in the first sector of the
   executable */
struct pksx_executable_header {
  /* Should be "SC" */
  uint8_t id[2];
  /* Should be 0x11, 0x12 or 0x13 for 1, 2 or 3 frames
     respectively. Other values are (probably?) invalid */
  uint8_t icon_frame_count;
  /* This seems to be the number of blocks used by the executable, I'm
     not sure how to get this value at compile time so it'll have to
     be patched later... */
  uint8_t block_count;
  /* Title in Shift-JIS character set */
  uint8_t title_shift_jis[64];
  /* Should be 0 */
  uint8_t reserved[12];
  /* Number of mono icons, if 0 the pocketstation kernel uses the
     executable icons */
  uint16_t pksx_mono_icons;
  /* Pocketstation identifier: "MCX0" or "MCX1" if there's a
     snapshot */
  uint8_t pksx_id[4];
  /* Number of executable icons (at least 1?) */
  uint8_t pksx_exe_icons;
  /* Number of entries in the function table */
  uint8_t pksx_functions;
  /* Should be zero */
  uint8_t pksx_reserved[4];
  /* Entry point */
  void (*pksx_start)(void);
  /* Icon palette */
  uint16_t icon_palette[16];
};

struct pksx_mono_icon_desc {
  /* Number of animation frames */
  uint16_t nframes;
  /* Delay between frames, in 1/30th of a second */
  uint16_t delay;
  /* Pointer to the frame data */
  const uint32_t *data;
};

#endif /* _POCKETSTATION_H_ */
