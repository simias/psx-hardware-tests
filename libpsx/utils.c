#include "utils.h"

void *memset(void *s, int c, size_t n) {
    char *p = s;
    uint32_t ci;

    c &= 0xff;

    ci = c | (c << 8) | (c << 16) | (c << 24);

    /* First attempt to use word writes */
    while (n >= 4) {
        uint32_t *i = (void *)p;

        *i = ci;

        n -= 4;
        p += 4;
    }

    /* Finish with byte writes */
    while (n--) {
        *p++ = c;
    }

    return s;
}
