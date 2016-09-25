#include "runtime.h"

void gpu_gp0(uint32_t cmd) {
  write32(0x1f801810, cmd);
}

void gpu_gp0_sync(uint32_t cmd) {
  bios_gpu_sync();

  gpu_gp0(cmd);
}

void gpu_gp1(uint32_t cmd) {
  write32(0x1f801814, cmd);
}

void gpu_gp0_cmd(const uint32_t *cmd, size_t len) {
  size_t i;

  gpu_gp0_sync(cmd[0]);

  for (i = 1; i < len; i++) {
    gpu_gp0(cmd[i]);
  }
}

void gpu_fill_rect(uint32_t color,
		   uint16_t x, uint16_t y,
		   uint16_t w, uint16_t h) {

  uint32_t cmd = 0x02000000;

  cmd |= color & 0xffffff;

  gpu_gp0_sync(cmd);
  gpu_gp0(x | (y << 16));
  gpu_gp0(w | (h << 16));
}

void delay(size_t loops) {
  while (loops--) {
    nop();
  }
}

void upload_texture(uint16_t x, uint16_t y,
		    uint16_t w, uint16_t h,
		    const uint16_t *texture) {
  uint32_t len = w * h;

  gpu_gp0_sync(0xa0000000);
  gpu_gp0(x | (y << 16));
  gpu_gp0(w | (h << 16));

  while (len) {
    uint32_t w = *(texture++);

    if (len--) {
      w |= (*(texture++) << 16);
      len--;
    }

    gpu_gp0_sync(w);
  }
}

int main() {
  bios_printf("Texture test\n");

  /* Reset */
  gpu_gp1(0x00000000);

  /* Display on */
  gpu_gp1(0x03000000);

  /* Enable FIFO (?) */
  gpu_gp1(0x04000001);

  /* Display start 0, 0 */
  gpu_gp1(0x05000000);

  /* Horizontal display range*/
  gpu_gp1(0x06c60260);

  /* Vertical display range*/
  gpu_gp1(0x0703e018);

  /* Display mode NTSC 256x240 */
  gpu_gp1(0x08000000);

  /* Set drawing area top left at 0,0 */
  gpu_gp0_sync(0xe3000000);

  /* Set drawing area bottom right at 256,256 */
  gpu_gp0_sync(0xe4040100);

  /* Set drawing offset at 0,0 */
  gpu_gp0_sync(0xe5000000);

  /* Clear screen */
  gpu_fill_rect(0x000000, 0, 0, 256, 256);

#define RGB555(_r, _g, _b) (((_r) >> 3) | (((_g) >> 3) << 5) | (((_b) >> 3) << 10))
#define RED RGB555(0xff, 0, 0)
#define GRN RGB555(0, 0xff, 0)
#define BLU RGB555(0, 0, 0xff)
#define BLK RGB555(0, 0, 0)
#define WHT RGB555(0xff, 0xff, 0xff)
#define BGD RGB555(0x12, 0x24, 0x74)
#define LTL RGB555(0x12, 0xf5, 0)

  static const uint16_t texture[16 * 16] = {
    GRN, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, GRN,
    BLU, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, WHT, WHT, WHT, WHT, WHT, WHT, WHT, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, WHT, WHT, WHT, WHT, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, LTL, LTL, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, BGD, BGD, LTL, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, BGD, BGD, LTL, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, LTL, LTL, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, LTL, BGD, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, WHT, BGD, BGD, BGD, BGD, LTL, BGD, LTL, BGD, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, LTL, BGD, BGD, LTL, BGD, BGD, BLU,
    BLU, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BGD, BLU,
    GRN, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, RED, GRN,
  };

  upload_texture(128, 0, 16, 16, texture);

  static const uint32_t textured_quad[] = {
    /* Command + Color 0 */
    0x3cffffff,
    /* vertex 0 */
    0x00100010,
    /* uv0 + clut */
    0x00000000,
    /* Color 1 */
    0x000000ff,
    /* vertex 1 */
    0x00100050,
    /* uv1 + texpage (0x256, 16bpp) */
    0x01020010,
    /* Color 2 */
    0x0000ff00,
    /* vertex 2 */
    0x00500010,
    /* uv2 */
    0x00001000,
    /* Color 3 */
    0x00ff0000,
    /* vertex 3 */
    0x00500050,
    /* uv3 */
    0x00001010,
  };

  gpu_gp0_cmd(textured_quad, ARRAY_SIZE(textured_quad));

  delay(10000000);

  bios_printf("Done\n");

  return 0;
}
