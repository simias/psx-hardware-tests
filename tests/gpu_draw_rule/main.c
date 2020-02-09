#include "utils.h"
#include "bios.h"

void gpu_gp0(uint32_t cmd) {
    write32(0x1f801810, cmd);
}

uint32_t gpu_read() {
    return read32(0x1f801810);
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
    const uint16_t read_width = 10;
    const uint16_t read_height = 10;
    const uint16_t read_total = read_width * read_height;
    const uint16_t expected_words = (read_total + 1) / 2;
    uint16_t i;

    bios_printf("GPU draw rule test\n");

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

#define COORD(_x, _y) (((uint32_t)(_x) & 0xffff) | \
                       (((uint32_t)(_y) & 0xffff) << 16))
#if 0
    static const uint32_t draw_cmd1[] = {
        /* shaded triangle */
        0x300000ff,
        COORD(2, 2),
        0x00ff0000,
        COORD(8, 5),
        0x0000ff00,
        COORD(3, 5),
    };

    gpu_gp0_cmd(draw_cmd1, ARRAY_SIZE(draw_cmd1));

    static const uint32_t draw_cmd2[] = {
        /* shaded triangle */
        0x300000ff,
        COORD(2, 8),
        0x00ff0000,
        COORD(8, 5),
        0x0000ff00,
        COORD(3, 5),
    };

    gpu_gp0_cmd(draw_cmd2, ARRAY_SIZE(draw_cmd2));
#endif
    static const uint32_t draw_quad[] = {
        /* shaded triangle */
        0x200000ff,
        COORD(9, 2),
        COORD(2, 6),
        COORD(7, 6),
    };

    gpu_gp0_cmd(draw_quad, ARRAY_SIZE(draw_quad));

    static const uint32_t read_vram[] = {
        /* Command read */
        0xc0000000,
        /* Top-left coord */
        COORD(0, 0),
        /* Size in 16bit pixels */
        COORD(read_width, read_height),
    };

    gpu_gp0_cmd(read_vram, ARRAY_SIZE(read_vram));

    bios_printf("uint16_t pixel_data[%u * %u] = {\n", read_width, read_height);
    for (i = 0; i < expected_words; i++) {
        uint32_t r = gpu_read();
        uint16_t p1, p2;

        if (i != 0 && ((i * 2) % read_width == 0)) {
            bios_printf("\n");
        }

        p1 = r & 0xffff;
        p2 = r >> 16;

        if (p1 != 0) {
            bios_printf(" 0x%04x,", p1);
        } else {
            bios_printf("      0,");
        }
        if (p2 != 0) {
            bios_printf(" 0x%04x,", p2);
        } else {
            bios_printf("      0,");
        }
    }
    bios_printf("\n};\n");

    bios_printf("Done\n");

    return 0;
}
