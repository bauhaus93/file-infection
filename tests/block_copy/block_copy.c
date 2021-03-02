#include <assert.h>
#include <stdio.h>

#include "block/copy.h"
#include "block/discovery.h"
#include "code_begin.h"
#include "code_end.h"
#include "infection_thread.h"
#include "test_utility.h"
#include "utility.h"

void memdump(void *addr, size_t size, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (f != NULL) {
        fwrite(addr, 1, size, f);
        fclose(f);
    }
}

bool write_modified_code_copy(const char *src_binary,
                              const char *target_binary) {
    PRINT_DEBUG("Binary file: '%s'", src_binary);
    size_t size = 0;
    uint8_t *code = read_binary(src_binary, &size);
    if (code == NULL) {
        fprintf(stderr, "Could not read binary\n");
        return false;
    }
    PRINT_DEBUG("Size:\t\t%.1f kiB", ((float)size) / 1024.0);

    void *entrypoints[] = {(void *)code, NULL};

    BlockList *blocks = discover_blocks(entrypoints);

    if (blocks == NULL) {
        fprintf(stderr, "Could not discover blocks\n");
        return false;
    }

    size_t target_size = estimate_target_code_size(blocks);
    assert(target_size > 0);
    fprintf(stderr, "Target code size estimate = %.1fkB\n",
            (float)target_size / 1024.);

    PRINT_DEBUG("Allocating target memory");
    void *target_mem = malloc(target_size);
    assert(target_mem != NULL);
    PRINT_DEBUG("Target base is %p", target_mem);

    if (!copy_blocks(blocks, target_mem, target_size)) {
        fprintf(stderr, "Could not copy modified blocks\n");
        return false;
    }
    void *target_entrypoint = get_target_address(entrypoints[0], blocks);
    if (target_entrypoint == NULL) {
        fprintf(stderr, "Could not find target entry point\n");
        return false;
    }
    PRINT_DEBUG("Target entry point is %p", target_entrypoint);

    memdump(target_mem, target_size, target_binary);

    free(target_mem);
    free_block_list(blocks);
    return true;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr,
                "Need 2 args: Path of code binary dump, followed by output "
                "path of modified binary");
        return 1;
    }

    if (!write_modified_code_copy(argv[1], argv[2])) {
        fprintf(stderr, "Could not write modified code copy");
        return 1;
    }

    return 0;
}
