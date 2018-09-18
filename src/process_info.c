#include "process_info.h"

TEB* get_teb(void) {
    TEB* teb = NULL;
    if (IS_32_BIT) {
        asm("movl %%fs:0x18, %0" : "=r" (teb));
    } else {
        asm("movq %%gs:0x30, %0" : "=r" (teb));
    }
    assert(teb != NULL);
    return teb;
}

PEB* get_peb(void) {
    return get_teb()->peb;
}

void* get_image_base(void) {
    return get_peb()->imageBase;
}

void* get_kernel32_base(void) {
    PEBLdrData* ldr = get_peb()->ldr;
    LdrEntry* entry = (LdrEntry*)ldr->moduleList.flink->flink->flink; //assumes kernel32.dll is third in list
    return entry->dllBase;
}