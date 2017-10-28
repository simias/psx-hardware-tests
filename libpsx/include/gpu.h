#ifndef _GPU_H_
#define _GPU_H_

enum gpu_vmode {
    GPU_NTSC,
    GPU_PAL,
};

enum gpu_xres {
    GPU_XRES_256,
    GPU_XRES_320,
    GPU_XRES_368,
    GPU_XRES_512,
    GPU_XRES_640,
};

enum gpu_interlacing {
    GPU_INTERLACING_DISABLED,
    GPU_INTERLACING_ENABLED,
};

enum gpu_semi_transparency {
    GPU_TRANSP_HALF_SRC_PLUS_HALF_DST = 0,
    GPU_TRANSP_SRC_PLUS_DST = 1,
    GPU_TRANSP_DST_MINUS_SRC = 2,
    GPU_TRANSP_DST_MINUS_QUARTER_SRC = 3,
};

enum gpu_texture_depth {
    GPU_TEX_4BPP = 0,
    GPU_TEX_8BPP = 1,
    GPU_TEX_15BPP = 2,
};

enum gpu_dither {
    GPU_DITHER_DISABLED = 0,
    GPU_DITHER_ENABLED = 1,
};

enum gpu_draw {
    GPU_IGNORE_DISPLAY_AREA = 0,
    GPU_DRAW_TO_DISPLAY_AREA = 1,
};

void gpu_reset(void);
void gpu_set_video_mode(enum gpu_vmode vmode,
                        enum gpu_xres xres,
                        enum gpu_interlacing interlacing);

void gpu_display_enable(bool enable);

void gpu_set_draw_area(uint16_t x0, uint16_t y0,
                       uint16_t x1, uint16_t y1);

void gpu_set_draw_offset(int16_t x, int16_t y);

void gpu_upload_texture(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *tex);

void gpu_set_texpage(unsigned tex_x_64,
                     unsigned tex_y_256,
                     enum gpu_semi_transparency transp,
                     enum gpu_texture_depth depth,
                     enum gpu_dither dither,
                     enum gpu_draw draw);

void gpu_clear_cache(void);

void gpu_draw_rect_raw_texture_opaque(unsigned x, unsigned y,
                                      unsigned w, unsigned h,
                                      unsigned tex_x, unsigned tex_y,
                                      unsigned clut_x_16, unsigned clut_y);

void gpu_draw_rect_monochrome_opaque(unsigned x, unsigned y,
                                     unsigned w, unsigned h,
                                     uint8_t r, uint8_t g, uint8_t b);

#endif /* _GPU_H_ */
