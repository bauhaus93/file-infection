#include <assert.h>

#include "process_info.h"
#include "string_generator.h"
#include "utility.h"

TEB *get_teb(void) {
    TEB *teb = NULL;
    asm("mov %%fs:0x18, %0" : "=r"(teb));
    return teb;
}

PEB *get_peb(void) { return get_teb()->peb; }

void *get_module_base(uint16_t module_string_id) {
    char module_path[100];
    size_t str_len = get_string_length(module_string_id) + 1;
    if (get_string(module_string_id, module_path, 100) == 0) {
        PRINT_DEBUG("could not get string for id = %04X ", module_string_id);
        return NULL;
    }
    PEBLdrData *ldr = get_peb()->ldr;
    for (LdrEntry *entry = (LdrEntry *)ldr->module_list.flink;
         entry != (LdrEntry *)&ldr->module_list;
         entry = (LdrEntry *)entry->module_list.flink) {

        UnicodeStr *uni = &entry->dll_name;

        if (uni->max_len / 2 == str_len) {
            uint8_t same = 1;
            for (uint16_t i = 0; i < uni->max_len / 2; i++) {
                if (!same_case_insensitive((char)uni->buffer[i],
                                           module_path[i])) {
                    same = 0;
                    break;
                }
            }
            if (same) {
                return entry->dll_base;
            }
        }
    }
    return NULL;
}

void *get_kernel32_base(void) { return get_module_base(STRING_KERNEL32_PATH); }
