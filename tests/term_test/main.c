#include "utils.h"
#include "bios.h"
#include "term.h"

int main() {
    unsigned i;

    term_init(GPU_XRES_256, GPU_NTSC, GPU_INTERLACING_DISABLED, 100);

    term_hook_bios_puchar();

    bios_printf("Hello \x1b[32;41mterm\x1b[0m\n");

    for (i = 0; i < 20000000; i++) {
        nop();
    }

    term_close();

    bios_printf("Done!\n");
}
