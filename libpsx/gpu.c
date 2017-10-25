#include "utils.h"
#include "gpu.h"
#include "bios.h"

#define GPU_BASE 0x1f801810
#define GPU_GP0  (GPU_BASE + 0)
#define GPU_GP1  (GPU_BASE + 4)
#define GPU_READ (GPU_BASE + 0)
#define GPU_STAT (GPU_BASE + 4)

void gpu_reset(void) {
    write32(GPU_GP1, 0);
}

static bool gpu_gp0_ready(void) {
    return !!(read32(GPU_STAT) & (1UL << 28));
}

static void gpu_write_gp0(uint32_t v) {
    while (!gpu_gp0_ready()) {
        ;
    }

    write32(GPU_GP0, v);
}

static void gpu_write_gp0_cmd(const uint32_t *cmd,
                              size_t n) {
    while (!gpu_gp0_ready()) {
        ;
    }

    while (n--) {
        write32(GPU_GP0, *cmd++);
    }
}

const uint32_t cmd[] = {
    0x38000000, 0x00000000, 0x00ff0000, 0x00000100,
    0x0000ff00, 0x01000000, 0x000000ff, 0x01000100,
};

void gpu_set_video_mode(enum gpu_vmode vmode,
                        enum gpu_xres xres,
                        enum gpu_interlacing interlacing) {

    /* GP1 opcode 0x08: Display mode */
    uint32_t r = 0x08UL << 24;

    static const uint32_t xres_reg[] = {
        [GPU_XRES_256] = 0,
        [GPU_XRES_320] = 1,
        [GPU_XRES_368] = 1 << 6,
        [GPU_XRES_512] = 2,
        [GPU_XRES_640] = 3,
    };

    if (xres < ARRAY_SIZE(xres_reg)) {
        r |= xres_reg[xres];
    }

    if (vmode == GPU_PAL) {
        r |= 1 << 3;
    }

    if (interlacing == GPU_INTERLACING_ENABLED) {
        r |= 1 << 2;
        r |= 1 << 5;
    }

    write32(GPU_GP1, r);
}

void gpu_display_enable(bool enable) {
    /* GP1 opcode 0x03: Display enable */
    uint32_t r = 0x03UL << 24;

    r |= !enable;

    write32(GPU_GP1, r);


    bios_printf("Hello %s:%d\n", __func__,  __LINE__);

    /*
      GP0(E3h) - Set Drawing Area top left (X1,Y1)
      GP0(E4h) - Set Drawing Area bottom right (X2,Y2)

      0-9    X-coordinate (0..1023)
      10-18  Y-coordinate (0..511)   ;\on Old 160pin GPU (max 1MB VRAM)
      19-23  Not used (zero)         ;/
      10-19  Y-coordinate (0..1023)  ;\on New 208pin GPU (max 2MB VRAM)
      20-23  Not used (zero)         ;/(retail consoles have only 1MB though)
      24-31  Command  (Exh)
    */

    gpu_write_gp0(0xe3000000UL);
    bios_printf("Hello %s:%d\n", __func__,  __LINE__);
    gpu_write_gp0(0xe4040100UL);
    bios_printf("Hello %s:%d\n", __func__,  __LINE__);
    gpu_write_gp0(0xe5000000UL);

    bios_printf("Hello %s:%d\n", __func__,  __LINE__);

    gpu_write_gp0(0xe1000000 | (1U << 10));

    gpu_write_gp0_cmd(cmd, ARRAY_SIZE(cmd));

    /*GP0(02h) - Fill Rectangle in VRAM

  1st  Color+Command     (CcBbGgRrh)  ;24bit RGB value (see note)
  2nd  Top Left Corner   (YyyyXxxxh)  ;Xpos counted in halfwords, steps of 10h
  3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords, steps of 10h
    */

    /* gpu_write_gp0(0x020000ff); */
    /* gpu_write_gp0(0x00000000); */
    /* gpu_write_gp0(0x01000100); */

    /*
      GP0(60h) - Monochrome Rectangle (variable size) (opaque)
GP0(62h) - Monochrome Rectangle (variable size) (semi-transparent)
GP0(68h) - Monochrome Rectangle (1x1) (Dot) (opaque)
GP0(6Ah) - Monochrome Rectangle (1x1) (Dot) (semi-transparent)
GP0(70h) - Monochrome Rectangle (8x8) (opaque)
GP0(72h) - Monochrome Rectangle (8x8) (semi-transparent)
GP0(78h) - Monochrome Rectangle (16x16) (opaque)
GP0(7Ah) - Monochrome Rectangle (16x16) (semi-transparent)

  1st  Color+Command     (CcBbGgRrh)
  2nd  Vertex            (YyyyXxxxh)
 (3rd) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
    */

    /* bios_printf("Hello %s:%d\n", __func__,  __LINE__); */
    /* gpu_write_gp0(0x78abcdef); */
    /* bios_printf("Hello %s:%d\n", __func__,  __LINE__); */
    /* gpu_write_gp0(0x00000000); */
    /* bios_printf("Hello %s:%d\n", __func__,  __LINE__); */
}

void gpu_upload_texture(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *tex) {
    /*
GP0(A0h) - Copy Rectangle (CPU to VRAM)

  1st  Command           (Cc000000h)
  2nd  Destination Coord (YyyyXxxxh)  ;Xpos counted in halfwords
  3rd  Width+Height      (YsizXsizh)  ;Xsiz counted in halfwords
  ...  Data              (...)      <--- usually transferred via DMA
    */
    size_t nwords;
    uint32_t cmd[3];

    cmd[0] = 0xa0000000;
    cmd[1] = (y << 16) | x;
    cmd[2] = (h << 16) | w;

    gpu_write_gp0_cmd(cmd, ARRAY_SIZE(cmd));

    nwords = (w * h + 1) / 2;

    while (nwords--) {
        uint16_t b0 = *tex++;
        uint16_t b1 = *tex++;

        uint32_t p = b0 | (b1 << 16);

        gpu_write_gp0(p);
    }

    /* GP0(E1h) - Draw Mode setting (aka "Texpage") */

/*   0-3   Texture page X Base   (N*64) (ie. in 64-halfword steps)    ;GPUSTAT.0-3 */
/*   4     Texture page Y Base   (N*256) (ie. 0 or 256)               ;GPUSTAT.4 */
/*   5-6   Semi Transparency     (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)   ;GPUSTAT.5-6 */
/*   7-8   Texture page colors   (0=4bit, 1=8bit, 2=15bit, 3=Reserved);GPUSTAT.7-8 */
/*   9     Dither 24bit to 15bit (0=Off/strip LSBs, 1=Dither Enabled) ;GPUSTAT.9 */
/*   10    Drawing to display area (0=Prohibited, 1=Allowed)          ;GPUSTAT.10 */
/*   11    Texture Disable (0=Normal, 1=Disable if GP1(09h).Bit0=1)   ;GPUSTAT.15 */
/*           (Above might be chipselect for (absent) second VRAM chip?) */
/*   12    Textured Rectangle X-Flip   (BIOS does set this bit on power-up...?) */
/*   13    Textured Rectangle Y-Flip   (BIOS does set it equal to GPUSTAT.13...?) */
/*   14-23 Not used (should be 0) */
/*   24-31 Command  (E1h) */


    gpu_write_gp0(0xe100000f | (1U << 10));

    /*

GP0(64h) - Textured Rectangle, variable size, opaque, texture-blending
GP0(65h) - Textured Rectangle, variable size, opaque, raw-texture
GP0(66h) - Textured Rectangle, variable size, semi-transp, texture-blending
GP0(67h) - Textured Rectangle, variable size, semi-transp, raw-texture
GP0(6Ch) - Textured Rectangle, 1x1 (nonsense), opaque, texture-blending
GP0(6Dh) - Textured Rectangle, 1x1 (nonsense), opaque, raw-texture
GP0(6Eh) - Textured Rectangle, 1x1 (nonsense), semi-transp, texture-blending
GP0(6Fh) - Textured Rectangle, 1x1 (nonsense), semi-transp, raw-texture
GP0(74h) - Textured Rectangle, 8x8, opaque, texture-blending
GP0(75h) - Textured Rectangle, 8x8, opaque, raw-texture
GP0(76h) - Textured Rectangle, 8x8, semi-transparent, texture-blending
GP0(77h) - Textured Rectangle, 8x8, semi-transparent, raw-texture
GP0(7Ch) - Textured Rectangle, 16x16, opaque, texture-blending
GP0(7Dh) - Textured Rectangle, 16x16, opaque, raw-texture
GP0(7Eh) - Textured Rectangle, 16x16, semi-transparent, texture-blending
GP0(7Fh) - Textured Rectangle, 16x16, semi-transparent, raw-texture

  1st  Color+Command     (CcBbGgRrh) (color is ignored for raw-textures)
  2nd  Vertex            (YyyyXxxxh) (upper-left edge of the rectangle)
  3rd  Texcoord+Palette  (ClutYyXxh) (for 4bpp Textures Xxh must be even!)
 (4th) Width+Height      (YsizXsizh) (variable size only) (max 1023x511)
    */

    //GP0(01h) - Clear Cache
    gpu_write_gp0(0x01000000);


    if (w <= 16) {
        uint32_t quad[4];
        uint16_t clut = (960 / 16) | (0x2f << 6);

        quad[0] = 0x65000000;
        quad[1] = 0x00300000;
        quad[2] = clut << 16;
        quad[3] = 0x000a00c0;

        gpu_write_gp0_cmd(quad, ARRAY_SIZE(quad));
    }
}
