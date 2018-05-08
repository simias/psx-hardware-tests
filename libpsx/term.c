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
static const uint16_t clut[] = {
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
#undef RGB

#define RGB(_r, _g, _b) ((_r) | (_g) << 8 | (_b) << 16)
static const uint32_t bg_color[] = {
    /* Black */
    RGB(0, 0, 0),
    /* Red */
    RGB(170, 0, 0),
    /* Green */
    RGB(0, 170, 0),
    /* Yellow */
    RGB(170, 85, 0),
    /* Blue */
    RGB(0, 0, 170),
    /* Magenta */
    RGB(170, 0, 170),
    /* Cyan */
    RGB(0, 170, 170),
    /* White */
    RGB(170, 170, 170),
};
#undef RGB

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
    /* The character itself (ASCII representation). *Must* be a
       printable character or \0 if the character is blank. */
    uint8_t c;
    /* Style associated with this character. */
    uint8_t style;
};

struct {
    /* Width of the terminal in number of pixels */
    uint16_t width_px;
    /* Height of the terminal in number of pixels */
    uint16_t height_px;
    /* Width of the terminal in number of characters */
    uint16_t width_char;
    /* Height of the terminal in number of lines */
    uint16_t height_line;

    /* Circular buffer holding the entire contents of the terminal (displayed
       lines + backbuffer) */
    struct term_char *char_buf;
    /* Total number of lines in the buffer (displayed + back buffer) */
    uint16_t buf_lines;
    /* Cursor position on the last line of the buffer */
    uint16_t cursor_column;
    /* One past the index of the first (oldest) line in the buffer */
    uint16_t buf_start_line;
    /* Index of the last (newest) line in the buffer*/
    uint16_t buf_end_line;
    /* Current draw style (color, bolded etc...). Updated when
       encountering ANSI escape sequences in the character stream. */
    uint8_t  cur_style;
    /* State machine for the ANSI escape sequence parser */
    enum term_ansi_parser_state ansi_parser_state;
    /* Accumulator variable used by the ANSI parser state machine */
    uint8_t ansi_parser_acc;
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
    term_context.height_line = term_context.height_px / FONT_HEIGHT;

    term_context.buf_lines = term_context.height_line + backbuffer_lines;

    term_context.buf_start_line = term_context.buf_end_line = 0;

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
            term_context.ansi_parser_acc = 0;
            return 1;
        } else {
            term_context.ansi_parser_state = TERM_ANSI_NO_CODE;
            return 0;
        }
    case TERM_ANSI_HAS_CARRET:
        if (c >= '0' && c <= '9') {
            term_context.ansi_parser_acc *= 10;
            term_context.ansi_parser_acc += c - '0';
            return 1;
        }

        if (c == ';' || c == 'm') {
            uint8_t code = term_context.ansi_parser_acc;

            if (code == 0) {
                /* Reset */
                term_context.cur_style = TERM_STYLE_DEFAULT;
            } else if (code == 1) {
                /* Set "bright" or "bold" */
                term_context.cur_style |= TERM_STYLE_BOLD_BIT;
            } else if (code >= 30 && code <= 37) {
                /* Foreground color */
                term_context.cur_style &= ~TERM_STYLE_FG_MASK;
                term_context.cur_style |= code - 30;
            } else if (code >= 40 && code <= 47) {
                /* Background color */
                term_context.cur_style &= ~TERM_STYLE_BG_MASK;
                term_context.cur_style |= (code - 40) << 4;
            }

            term_context.ansi_parser_acc = 0;

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

/* Returns a pointer to the char at the current cursor position
   (i.e. the one that will be written to next) */
static struct term_char *term_char_at_cursor(void) {
    unsigned pos = term_context.buf_end_line * term_context.width_char;

    pos += term_context.cursor_column;

    return &term_context.char_buf[pos];
}

void term_putchar(int c) {
    struct term_char *tc;

    if (term_parse_ansi_code(c)) {
        /* This was part of an ASCII escape sequence, we don't have
           anything to display */
        return;
    }

    if (c == '\n') {
        term_newline();
        return;
    }

    if (c >= ' ' && c <= '~') {
        /* Non printable character */
        return;
    }

    tc = term_char_at_cursor();
    tc->style = term_context->cur_style;
    tc->c = c;

    /* Move the cursor */
    if ((term_context.cursor_column + 1) >= term_context.width_char) {
        /* cursor reached the end of the line, move to the next one. */
        term_newline();
    } else {
        /* We're still on the same line. We can just draw the
           new character where it belongs and we're done. */
        draw_char_at_cursor();

        term_context.cursor_column++;
    }
}

/* Move to a new empty line. */
void term_newline(void) {
    struct term_char *tc;

    term->buf_end_line++;
    term->cursor_column = 0;

    /* Clear the new line */
    tc = term_char_at_cursor();
    memset(tc, 0, term_context.width_char * sizeof(struct term_char));

    /* Redraw the entire screen. We could optimize this by copying the
       existing screen upwards and only drawing a blank line at the
       bottom. */
    term_redraw_screen();
}

/* Redraw the entire screen */
void term_redraw_screen(void) {
    /* I expect that most of the time there'll be a lot of blank space
       on the screen (partially written lines, spaces etc...) so I
       start by clearing the entire screen in a single command and
       then I'll just skip over whitespace. */
    gpu_draw_rect_monochrome_opaque(0, 0,
                                    term_context.width_px,
                                    term_context.height_px,
                                    0, 0, 0);

    for (uint16_t screen_y = 0;
         screen_y < term_context.height_line;
         screen_y++) {
        
    }
}

void term_display_char(struct term_char tc, uint16_t screen_x, uint16_t screen_y) {
    unsigned tex_x;
    unsigned tex_y;
    unsigned x;
    unsigned y;
    unsigned fg_clut;
    uint32_t bg_col;

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

    bg_col = bg_color[(term_context.cur_style & TERM_STYLE_BG_MASK) >> 4];

    x = term_context.cursor_pos * (FONT_WIDTH + 1);
    y = 20;

    gpu_draw_rect_monochrome_opaque(x, y,
                                    FONT_WIDTH + 1, FONT_HEIGHT,
                                    bg_col & 0xff,
                                    (bg_col >> 8) & 0xff,
                                    (bg_col >> 16) & 0xff);

    gpu_draw_rect_raw_texture_opaque(x, y,
                                     FONT_WIDTH, FONT_HEIGHT,
                                     tex_x, tex_y,
                                     960 / 16, fg_clut);
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
