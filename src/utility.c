#include "utility.h"

uint8_t same_case_insensitive(char a, char b) {
    if (a >= 'A' && a <= 'Z') {
        a += 0x20;
    }
    if (b >= 'A' && b <= 'Z') {
        b += 0x20;
    }
    return a == b;
}
