#include "checksum.h"

// https://en.wikipedia.org/wiki/Adler-32
uint32_t checksum(const char *str) {
    uint32_t a = 1, b = 0;

    for (const char *ptr = str; *ptr != 0; ptr++) {
        a = (a + ((uint32_t)*ptr)) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) + a;
}
