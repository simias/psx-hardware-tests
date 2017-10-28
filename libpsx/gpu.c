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

void gpu_set_video_mode(enum gpu_vmode vmode,
                        enum gpu_xres xres,
                        enum gpu_interlacing interlacing) {

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

void gpu_set_draw_area(uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1) {
    x0 &= 0x3ff;
    x1 &= 0x3ff;
    y0 &= 0x1ff;
    y1 &= 0x1ff;

    gpu_write_gp0(0xe3000000UL | x0 | (y0 << 10));
    gpu_write_gp0(0xe4000000UL | x1 | (y1 << 10));
}

void gpu_set_draw_offset(int16_t x, int16_t y) {
    uint16_t ux = x;
    uint16_t uy = y;

    ux &= 0x7ff;
    uy &= 0x7ff;

    gpu_write_gp0(0xe5000000UL | x | (y << 11));
}

void gpu_display_enable(bool enable) {
    uint32_t r = 0x03UL << 24;

    r |= !enable;

    write32(GPU_GP1, r);
}

void gpu_upload_texture(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *tex) {
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
}

void gpu_set_texpage(unsigned tex_x_64,
                     unsigned tex_y_256,
                     enum gpu_semi_transparency transp,
                     enum gpu_texture_depth depth,
                     enum gpu_dither dither,
                     enum gpu_draw draw) {
    uint32_t r;

    r = 0xe1000000;

    r |= tex_x_64 & 0xf;
    r |= (tex_y_256 & 1) << 4;
    r |= transp << 5;
    r |= depth << 7;
    r |= dither << 9;
    r |= draw << 10;

    gpu_write_gp0(r);
}

void gpu_clear_cache(void) {
    gpu_write_gp0(0x01000000);
}

void gpu_draw_rect_raw_texture_opaque(unsigned x, unsigned y,
                                      unsigned w, unsigned h,
                                      unsigned tex_x, unsigned tex_y,
                                      unsigned clut_x_16, unsigned clut_y) {
    uint32_t rect[4];

    uint16_t clut = clut_x_16 | (clut_y << 6);
    uint16_t tex = tex_x | (tex_y << 8);

    rect[0] = 0x65000000;
    rect[1] = x | (y << 16);
    rect[2] = (clut << 16) | tex;
    rect[3] = w | (h << 16);

    gpu_write_gp0_cmd(rect, ARRAY_SIZE(rect));
}

void gpu_draw_rect_monochrome_opaque(unsigned x, unsigned y,
                                     unsigned w, unsigned h,
                                     uint8_t r, uint8_t g, uint8_t b) {
    uint32_t rect[3];


    rect[0] = 0x60000000 | r | (g << 8) | (b < 16);
    rect[1] = x | (y << 16);
    rect[2] = w | (h << 16);

    gpu_write_gp0_cmd(rect, ARRAY_SIZE(rect));
}
