#include "utils.h"
#include "bios.h"
#include "term.h"

#define FONT_WIDTH  3
#define FONT_HEIGHT 5

/* 4bpp 3x5 pixel font (texture size: 192 x 10, 480 16bit words) */
const uint16_t font[(192 * 10) / 4] = {
    0x0000, 0x0101, 0x1011, 0x1110, 0x1010, 0x0100, 0x1100, 0x0000,
    0x0000, 0x0000, 0x0000, 0x1000, 0x0111, 0x1101, 0x0110, 0x1101,
    0x1111, 0x1111, 0x1010, 0x0011, 0x0000, 0x0000, 0x0000, 0x1110,
    0x0111, 0x1101, 0x1100, 0x1011, 0x1111, 0x1101, 0x1101, 0x0011,
    0x1011, 0x1001, 0x1110, 0x0100, 0x1111, 0x1111, 0x1111, 0x1111,
    0x0110, 0x1011, 0x1101, 0x1110, 0x0111, 0x0001, 0x1011, 0x0000,
    0x0000, 0x0101, 0x1111, 0x0011, 0x0110, 0x0101, 0x0010, 0x0101,
    0x0101, 0x0000, 0x0000, 0x1000, 0x1101, 0x0001, 0x1001, 0x1101,
    0x0100, 0x1000, 0x1101, 0x1010, 0x0100, 0x1010, 0x1011, 0x1000,
    0x1101, 0x0110, 0x0011, 0x1101, 0x0100, 0x0010, 0x0101, 0x0001,
    0x1011, 0x1001, 0x0111, 0x1011, 0x1101, 0x0110, 0x0011, 0x1010,
    0x0110, 0x1011, 0x1101, 0x0010, 0x0011, 0x0001, 0x0110, 0x0001,
    0x0000, 0x0001, 0x1010, 0x0010, 0x1001, 0x0001, 0x0011, 0x1011,
    0x1110, 0x1000, 0x0011, 0x0100, 0x0101, 0x1001, 0x0110, 0x1111,
    0x1111, 0x0101, 0x1010, 0x0011, 0x0000, 0x0001, 0x0000, 0x1101,
    0x1100, 0x1111, 0x0010, 0x1101, 0x1101, 0x1010, 0x0111, 0x0001,
    0x0111, 0x1001, 0x0110, 0x1011, 0x1111, 0x1110, 0x1111, 0x1010,
    0x0110, 0x1011, 0x0010, 0x1001, 0x0010, 0x0010, 0x0010, 0x0000,
    0x0000, 0x0000, 0x1110, 0x1110, 0x1100, 0x0000, 0x0010, 0x0101,
    0x0101, 0x0010, 0x0000, 0x0010, 0x0101, 0x0101, 0x1000, 0x0100,
    0x0110, 0x0011, 0x0101, 0x1010, 0x0100, 0x1010, 0x1011, 0x0000,
    0x1111, 0x0110, 0x0011, 0x1101, 0x0100, 0x1010, 0x0101, 0x0101,
    0x1011, 0x1001, 0x0110, 0x1011, 0x1001, 0x1111, 0x1000, 0x1010,
    0x0110, 0x1111, 0x0101, 0x0101, 0x0010, 0x0100, 0x0010, 0x0000,
    0x0000, 0x0001, 0x1010, 0x1011, 0x1010, 0x0001, 0x1100, 0x0000,
    0x0000, 0x0001, 0x1000, 0x0010, 0x1111, 0x1111, 0x0111, 0x1100,
    0x1111, 0x0011, 0x1010, 0x0011, 0x0010, 0x0000, 0x0000, 0x0100,
    0x1111, 0x1010, 0x1101, 0x1011, 0x0111, 0x0100, 0x1101, 0x1111,
    0x1011, 0x1111, 0x0110, 0x0101, 0x0001, 0x0110, 0x1111, 0x1010,
    0x1011, 0x1010, 0x0101, 0x1101, 0x0111, 0x0100, 0x0011, 0x1110,
    0x0001, 0x0100, 0x0000, 0x0100, 0x1000, 0x0001, 0x0001, 0x0000,
    0x0011, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x1100, 0x1010, 0x0001, 0x1110,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0010, 0x0101, 0x1100, 0x0100, 0x1001, 0x0100, 0x0001, 0x0001,
    0x0010, 0x1010, 0x1111, 0x0100, 0x0011, 0x0111, 0x0100, 0x1010,
    0x0110, 0x1011, 0x1101, 0x1110, 0x0101, 0x0010, 0x0001, 0x1001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x1000, 0x1110, 0x0010, 0x1110, 0x1110, 0x1011, 0x0011, 0x0000,
    0x1011, 0x1010, 0x0111, 0x1011, 0x1101, 0x1110, 0x0010, 0x1111,
    0x0110, 0x1011, 0x1010, 0x1010, 0x0111, 0x0010, 0x1111, 0x1001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x1000, 0x0110, 0x0011, 0x1101, 0x1001, 0x1100, 0x0101, 0x0001,
    0x0111, 0x1010, 0x0110, 0x1011, 0x0011, 0x0111, 0x0100, 0x1010,
    0x0110, 0x1111, 0x0010, 0x1101, 0x0100, 0x0010, 0x0101, 0x0010,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x1111, 0x1100, 0x0110, 0x1011, 0x0010, 0x0101, 0x1001,
    0x1010, 0x1110, 0x0110, 0x0101, 0x0001, 0x0110, 0x0010, 0x1110,
    0x1011, 0x1110, 0x1101, 0x1101, 0x1101, 0x1010, 0x0001, 0x0110,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

/* Macro to convert 888 RGB into 555 BGR */
#define RGB(_r, _g, _b) ((_r >> 3) |         \
                         ((_g >> 3) << 5) |  \
                         ((_b >> 3) << 10))

/* 2 x 16 texture containing 2bit CLUTs for each of the 16 ANSI
   colors. The first entry (background color) is always fully
   transparent. */
const uint16_t clut[] = {
    /* 0x8000, 0xffff, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, */
    /* 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, */

    /* Black */
    0x0000, RGB(0, 0, 0),
    /* Red */
    0x0000, RGB(170, 0, 0),
    /* Green */
    0x0000, RGB(0, 170, 0),
    /* Yellow */
    0x0000, RGB(170, 85, 0),
    /* Blue */
    0x0000, RGB(0, 0, 170),
    /* Magenta */
    0x0000, RGB(170, 0, 170),
    /* Cyan */
    0x0000, RGB(0, 170, 170),
    /* White */
    0x0000, RGB(170, 170, 170),

    /* Bright black */
    0x0000, RGB(85, 85, 85),
    /* Bright Red */
    0x0000, RGB(255, 85, 85),
    /* Bright Green */
    0x0000, RGB(85, 255, 85),
    /* Bright Yellow */
    0x0000, RGB(255, 255, 85),
    /* Bright Blue */
    0x0000, RGB(85, 85, 255),
    /* Bright Magenta */
    0x0000, RGB(255, 85, 255),
    /* Bright Cyan */
    0x0000, RGB(85, 255, 255),
    /* Bright White */
    0x0000, RGB(255, 255, 255),
};

static void term_gpu_init(enum gpu_xres xres,
                          enum gpu_vmode vmode,
                          enum gpu_interlacing interlacing,
                          uint16_t width,
                          uint16_t height) {
    gpu_reset();

    gpu_set_video_mode(vmode, xres, interlacing);


    gpu_upload_texture(960, 0,
                       48, 10,
                       font);

    gpu_upload_texture(960, 0x20,
                       2, 16,
                       clut);

    gpu_clear_cache();

    gpu_set_draw_area(0, 0, width, height);

    gpu_set_draw_offset(0, 0);

    gpu_display_enable(true);

    gpu_set_texpage(960 / 64, 0,
                    GPU_TRANSP_HALF_SRC_PLUS_HALF_DST,
                    GPU_TEX_4BPP,
                    GPU_DITHER_DISABLED,
                    GPU_DRAW_TO_DISPLAY_AREA);

    gpu_draw_rect_monochrome_opaque(0, 0,
                                    width, height,
                                    0, 0, 0);
}

enum term_ansi_parser_state {
    TERM_ANSI_NO_CODE = 0,
    TERM_ANSI_HAS_ESC,
    TERM_ANSI_HAS_CARRET,
};

struct term_char {
    uint8_t c;
    uint8_t style;
};

struct {
    uint16_t width_px;
    uint16_t height_px;
    uint16_t width_char;
    uint16_t height_char;

    struct term_char *char_buf;
    uint16_t buf_lines;
    uint16_t cursor_pos;
    uint16_t buf_start;
    uint16_t buf_end;
    uint8_t  cur_style;

    enum term_ansi_parser_state ansi_parser_state;
    uint8_t ansi_parser_temp;
} term_context = {0};

/* Black bg, white fg */
#define TERM_STYLE_DEFAULT  0x07U
#define TERM_STYLE_BOLD_BIT 0x08U
#define TERM_STYLE_FG_MASK  0x07U
#define TERM_STYLE_BG_MASK  0x70U

static const unsigned term_width[] = {
    [GPU_XRES_256] = 256,
    [GPU_XRES_320] = 320,
    [GPU_XRES_368] = 368,
    [GPU_XRES_512] = 512,
    [GPU_XRES_640] = 640,
};

int term_init(enum gpu_xres xres,
              enum gpu_vmode vmode,
              enum gpu_interlacing interlacing,
              unsigned long backbuffer_lines) {

    size_t buf_len;

    term_close();

    if (xres > ARRAY_SIZE(term_width)) {
        bios_printf("%s: unknown xres: %d\n", __func__, xres);
        return -1;
    }

    term_context.width_px = term_width[xres];
    term_context.height_px = 240;

    if (interlacing == GPU_INTERLACING_ENABLED) {
        term_context.height_px *= 2;
    }

    term_context.ansi_parser_state = TERM_ANSI_NO_CODE;

    term_context.width_char = term_context.width_px / FONT_WIDTH;
    term_context.height_char = term_context.height_px / FONT_HEIGHT;

    term_context.buf_lines = term_context.height_char + backbuffer_lines;

    term_context.buf_start = term_context.buf_end = 0;

    term_context.cur_style = TERM_STYLE_DEFAULT;

    buf_len = term_context.buf_lines * term_context.width_char;
    buf_len *= sizeof(*term_context.char_buf);

    term_context.char_buf = bios_malloc(buf_len);

    if (term_context.char_buf == NULL) {
        bios_printf("%s: malloc failed\n", __func__);
        return -1;

    }

    memset(term_context.char_buf, 0, buf_len);

    term_gpu_init(xres, vmode, interlacing,
                  term_context.width_px, term_context.height_px);

    return 0;
}

/* Handles the ANSI code parsing state machine. Returns true if the
   character is part of a code and shouldn't be displayed */
int term_parse_ansi_code(int c) {
    switch (term_context.ansi_parser_state) {
    case TERM_ANSI_NO_CODE:
        if (c == 0x1b) {
            term_context.ansi_parser_state = TERM_ANSI_HAS_ESC;
            return 1;
        }
        return 0;

    case TERM_ANSI_HAS_ESC:
        if (c == '[') {
            term_context.ansi_parser_state = TERM_ANSI_HAS_CARRET;
            term_context.ansi_parser_temp = 0;
            return 1;
        } else {
            term_context.ansi_parser_state = TERM_ANSI_NO_CODE;
            return 0;
        }
    case TERM_ANSI_HAS_CARRET:
        if (c >= '0' && c <= '9') {
            term_context.ansi_parser_temp *= 10;
            term_context.ansi_parser_temp += c - '0';
            return 1;
        }

        if (c == ';' || c == 'm') {
            uint8_t tmp = term_context.ansi_parser_temp;

            if (tmp == 0) {
                /* Reset */
                term_context.cur_style = TERM_STYLE_DEFAULT;
            } else if (tmp == 1) {
                /* Set "bright" or "bold" */
                term_context.cur_style |= TERM_STYLE_BOLD_BIT;
            } else if (tmp >= 30 && tmp <= 37) {
                /* Foreground color */
                term_context.cur_style &= ~TERM_STYLE_FG_MASK;
                term_context.cur_style |= tmp - 30;
            } else if (tmp >= 40 && tmp <= 47) {
                /* Background color */
                term_context.cur_style &= ~TERM_STYLE_BG_MASK;
                term_context.cur_style |= (tmp - 40) << 4;
            }

            term_context.ansi_parser_temp = 0;

            if (c == 'm') {
                /* End of code */
                term_context.ansi_parser_state = TERM_ANSI_NO_CODE;
            }

            return 1;
        }

        /* Unknown code */
        term_context.ansi_parser_state = TERM_ANSI_NO_CODE;
        return 0;
    }

    return 0;
}

void term_putchar(int c) {
    unsigned tex_x;
    unsigned tex_y;
    unsigned fg_clut;

    if (term_parse_ansi_code(c)) {
        return;
    }

    if (c >= ' ' && c <= '~') {
        c -= ' ';
    } else {
        /* non printable char, use the special "placeholder" character
           at the last position in the font. */
        c = '~' - ' ' + 1;
    }

    tex_x = (c % 64) * FONT_WIDTH;
    tex_y = (c / 64) * FONT_HEIGHT;

    fg_clut = 0x20;
    fg_clut += term_context.cur_style & TERM_STYLE_FG_MASK;

    if (term_context.cur_style & TERM_STYLE_BOLD_BIT) {
        fg_clut += 8;
    }

    gpu_draw_rect_raw_texture_opaque(term_context.cursor_pos * (FONT_WIDTH + 1), 20,
                                     FONT_WIDTH, FONT_HEIGHT,
                                     tex_x, tex_y,
                                     960 / 16, fg_clut);

    term_context.cursor_pos++;
}

void term_close(void) {
    term_unhook_bios_puchar();

    if (term_context.char_buf) {
        bios_free(term_context.char_buf);

        memset(&term_context, 0, sizeof(term_context));
    }
}

static int (*term_bios_putchar)(int) = NULL;

static int term_putchar_wrapper(int c) {
    term_putchar(c);

    if (term_bios_putchar) {
        return term_bios_putchar(c);
    }

    return c;
}

void term_hook_bios_puchar(void) {
    func_ptr_t *b0_table = bios_get_b0_table();

    term_bios_putchar = (void*)b0_table[0x3d];

    b0_table[0x3d] = (void*)term_putchar_wrapper;
}

void term_unhook_bios_puchar(void) {
    func_ptr_t *b0_table;

    if (term_bios_putchar == NULL) {
        return;
    }

    b0_table = bios_get_b0_table();
    b0_table[0x3d] = (void*)term_bios_putchar;

    term_bios_putchar = NULL;
}
