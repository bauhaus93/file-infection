#include <assert.h>
#include <errno.h>
#include <sys/mman.h>

#include "block/copy.h"
#include "block/discovery.h"
#include "test_code.h"
#include "utility.h"

typedef int (*fpEntryPoint)(int);

bool execute_copied_code(void *entrypoint, void *copy_begin, void *copy_end) {
    void *entrypoints[] = {entrypoint, NULL};
    BlockList *blocks = discover_blocks(entrypoints);
    if (blocks == NULL) {
        PRINT_DEBUG("Could not discover blocks");
        return false;
    }

    size_t target_size = estimate_target_code_size(blocks);
    assert(target_size > 0);
    PRINT_DEBUG("Target code size estimate = %dB", target_size);

    PRINT_DEBUG("Allocating target memory");
    void *target_mem =
        mmap(NULL, target_size, PROT_EXEC | PROT_READ | PROT_WRITE,
             MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (target_mem == MAP_FAILED) {
        int err = errno;
        PRINT_DEBUG("Could not allocate memory: errno = %d", err);
        free_block_list(blocks);
        return false;
    }
    PRINT_DEBUG("Target base is %p", target_mem);

    if (!copy_blocks(blocks, target_mem, target_size)) {
        PRINT_DEBUG("Could not copy modified blocks");
        free_block_list(blocks);
        munmap(target_mem, target_size);
        return false;
    }
    void *target_entrypoint = get_target_address(entrypoint, blocks);
    if (target_entrypoint == NULL) {
        PRINT_DEBUG("Could not find target entry point");
        free_block_list(blocks);
        munmap(target_mem, target_size);
        return false;
    }
    PRINT_DEBUG("Target entry point is %p", target_entrypoint);

    int unmodified_result = ((fpEntryPoint)entrypoint)(42);
    int modified_result = ((fpEntryPoint)target_entrypoint)(42);

    munmap(target_mem, target_size);
    free_block_list(blocks);

    if (unmodified_result != modified_result) {
        PRINT_DEBUG("Result of copied code not matching with original: "
                    "Expected = %d, Found = %d",
                    unmodified_result, modified_result);
        return false;
    }

    return true;
}

int main(int argc, char **argv) {

    if (!execute_copied_code(test_entry, test_begin, test_end)) {
        PRINT_DEBUG("Could not execute copied code!");
        return 1;
    }

    return 0;
}
