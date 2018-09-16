#include "ass.h"

void* get_tib(void) {
    assert(sizeof(void*) == 8);
    void* tib = NULL;
    asm("movl %%fs:0x18, %0" : "=r" (tib));
    assert(tib != NULL);
    return tib;
}

void* get_peb(void) {
    return (void*)*(uint8_t*)get_tib() + 0x30;
}

void* get_image_base(void) {
    uint8_t* base = get_peb();
    base = base + 0xC;  base = *base;
    base = *(base + 0x14);
    base = *(base + 0x10);
        
    /*
    movl %fs:0x30, %eax
    movl 0xC(%eax), %eax
    movl 0x14(%eax), %eax
    movl 0x10(%eax), %eax
    ret
    ;*/
    return (void*)base;
}
void* get_kernel32_base(void) {
    uint8_t* base = get_peb();
    base = (uint8_t*)*(base + 0xC);
    base = (uint8_t*)*(base + 0x14);
    base = (uint8_t*)**base;
    base = (uint8_t*)*(base + 0x10);
    /*__asm {
        movl %fs:0x30, %eax
        movl 0xC(%eax), %eax
        movl 0x14(%eax), %eax
        movl (%eax), %eax
        movl (%eax), %eax
        movl 0x10(%eax), %eax
        ret
    }*/
    return (void*)base;
}

void end_code(void) {}