#include "utils.h"
#include "bios.h"
#include "term.h"

int main() {
    unsigned i;

    bios_printf("start!\n");

    term_init(GPU_XRES_320, GPU_NTSC, GPU_INTERLACING_DISABLED, 100);

    for (i = 0; i < 100000000; i++) {
        nop();
    }

    term_close();

    bios_printf("Done!\n");
}
