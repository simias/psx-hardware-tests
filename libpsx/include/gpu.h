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

void gpu_reset(void);
void gpu_set_video_mode(enum gpu_vmode vmode,
                        enum gpu_xres xres,
                        enum gpu_interlacing interlacing);
void gpu_display_enable(bool enable);

void gpu_upload_texture(uint16_t x, uint16_t y,
                        uint16_t w, uint16_t h,
                        const uint16_t *tex);


#endif /* _GPU_H_ */
