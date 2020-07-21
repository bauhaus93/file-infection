#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "checksum.h"
#include "code_begin.h"
#include "code_end.h"
#include "delta.h"
#include "disasm/analysis/analysis.h"
#include "disasm/analysis/block.h"
#include "disasm/analysis/call.h"
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

int main(int argc, char **argv) {
    const char *function_names[] = {"get_string",
                                    "checksum",
                                    "get_delta_offset",
                                    "analyze_code",
                                    "setup_disasm",
                                    "next_instruction",
                                    "get_current_instruction",
                                    "get_function_pointer",
                                    "infect",
                                    "spawn_infection_thread",
                                    "infection_thread",
                                    "memzero",
                                    "memcp",
                                    "get_original_entry_point",
                                    "write_original_entry_point",
                                    "get_nt_header_by_base",
                                    "get_section_header",
                                    "get_last_section_header",
                                    "get_export_directory_by_base",
                                    "is_section_header_empty",
                                    "create_section_header",
                                    "get_image_base",
                                    "get_module_base",
                                    "get_kernel32_base",
                                    NULL};
    const void *function_addresses[] = {(void *)get_string,
                                        (void *)checksum,
                                        (void *)get_delta_offset,
                                        (void *)analyze_code,
                                        (void *)setup_disasm,
                                        (void *)next_instruction,
                                        (void *)get_current_instruction,
                                        (void *)get_function_pointer,
                                        (void *)infect,
                                        (void *)spawn_infection_thread,
                                        (void *)infection_thread,
                                        (void *)memzero,
                                        (void *)memcp,
                                        (void *)get_original_entry_point,
                                        (void *)write_original_entry_point,
                                        (void *)get_nt_header_by_base,
                                        (void *)get_section_header,
                                        (void *)get_last_section_header,
                                        (void *)get_export_directory_by_base,
                                        (void *)is_section_header_empty,
                                        (void *)create_section_header,
                                        (void *)get_image_base,
                                        (void *)get_module_base,
                                        (void *)get_kernel32_base,
                                        NULL};

    size_t expected = 0;
    while (function_addresses[expected] != NULL &&
           function_names[expected] != NULL) {
        expected++;
    }
    assert(function_addresses[expected] == function_names[expected]);
    expected--;
    size_t found = 0;
    bool *found_list = malloc(sizeof(bool) * expected);
    memset(found_list, false, sizeof(bool) * expected);

    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread, (void *)analyze_code};

    CodeAnalysis *analysis =
        analyze_code(entrypoints, 3, (void *)code_begin, (void *)code_end);

    if (analysis == NULL) {
        fprintf(stderr, "analyze_code() returned NULL");
        return 1;
    }

    for (BlockList *bl = analysis->block_list; bl != NULL; bl = bl->next) {
        if (bl->block == NULL) {
            fprintf(stderr, "BlockList has block with NULL");
            return 1;
        }
        void *addr = bl->block->start;
        for (int i = 0;
             function_addresses[i] != NULL && function_names[i] != NULL; i++) {
            if (addr == function_addresses[i]) {
                fprintf(stderr, "Function found: %p -> %-20s\n", addr,
                        function_names[i]);
                found_list[i] = true;
                found++;
            }
        }
    }
    if (found != expected) {
        fprintf(stderr, "Not found all expected functions: %d/%d\n", found,
                expected);
        for (size_t i = 0; i < expected; i++) {
            if (!found_list[i]) {
                fprintf(stderr, "Function not found: %-20s\n",
                        function_names[i]);
            }
        }
    }
	free_code_analysis(analysis);
	free(found_list);
    return found != expected;
}
