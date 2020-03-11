#include "delta.h"
#include "utility.h"

int32_t get_delta_offset(void) {
    int32_t offset;
    if (IS_32_BIT) {
        asm("call .base\n"
            ".base:\n"
            "pop %%edx\n"
            "sub $.base, %%edx\n"
            "mov %%edx, %0"
            : "=r"(offset));
    } else {
        asm("call .base\n"
            ".base:\n"
            "pop %%rdx\n"
            "sub $.base, %%rdx\n"
            "mov %%edx, %0"
            : "=r"(offset));
    }
    return offset;
}
