#include "utils.h"
#include "bios.h"
#include "term.h"

int main() {
    unsigned i;

    term_init(100, GPU_NTSC);

    for (i = 0; i < 10000000; i++) {
        nop();
    }

    bios_printf("Done!\n");
}
