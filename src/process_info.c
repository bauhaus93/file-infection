#include <assert.h>

#include "process_info.h"
#include "string_generator.h"
#include "utility.h"

TEB *get_teb(void) {
    TEB *teb = NULL;
    if (IS_32_BIT) {
        asm("movl %%fs:0x18, %0" : "=r"(teb));
    } else {
        asm("movq %%gs:0x30, %0" : "=r"(teb));
    }
    assert(teb != NULL);
    return teb;
}

PEB *get_peb(void) { return get_teb()->peb; }

void *get_image_base(void) { return get_peb()->imageBase; }

void *get_kernel32_base(void) {
    char kernel32_path[50];
    size_t str_len = get_string_length(STRING_KERNEL32_PATH) + 1;
    if (get_string(STRING_KERNEL32_PATH, kernel32_path, 50) == 0) {
        PRINT_DEBUG("could not get word for WORD_KERNEL32_PATH\n");
    }

    PEBLdrData *ldr = get_peb()->ldr;
    for (LdrEntry *entry = (LdrEntry *)ldr->moduleList.flink;
         entry != (LdrEntry *)&ldr->moduleList;
         entry = (LdrEntry *)entry->moduleList.flink) {
        UnicodeStr *uni = &entry->dllName;
        if (uni->maxLen / 2 == str_len) {
            uint8_t same = 1;
            for (uint16_t i = 0; i < uni->maxLen / 2; i++) {
                if ((char)uni->buffer[i] != kernel32_path[i]) {
                    same = 0;
                    break;
                }
            }
            if (same) {
                return entry->dllBase;
            }
        }
    }
    return NULL;
}
