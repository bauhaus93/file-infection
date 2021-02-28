#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "block/block.h"
#include "block/discovery.h"
#include "utility.h"

// Mainly function which are used for testing purposes
static const char *BLACKLIST[] = {"code_begin",
                                  "code_end",
                                  "init_disasm",
                                  "destroy_disasm",
                                  "get_current_instruction_size",
                                  "get_instruction_size",
                                  "print_current_instruction",
                                  "print_block",
                                  "print_blocks",
                                  "print_instruction",
                                  "copy_blocks_test",
                                  "get_code_size",
                                  "get_target_address",
                                  "estimate_target_code_size",
                                  "count_effective_blocks",
                                  "__x86.get_pc_thunk.dx",
                                  NULL};

static bool is_blacklisted(const char *name) {
    for (int i = 0; BLACKLIST[i] != NULL; i++) {
        if (strcmp(name, BLACKLIST[i]) == 0) {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "Needing 2 args: Path of code binary dump followed by "
                        "path of file containing the function map");
        return 1;
    }

    PRINT_DEBUG("Code file: %s", argv[1]);

    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        fprintf(stderr, "Could not open file '%s'", argv[1]);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    PRINT_DEBUG("Size:\t\t%.1f kiB", ((float)size) / 1024.0);

    uint8_t *code = (uint8_t *)malloc(sizeof(uint8_t) * size);
    if (code == NULL) {
        fprintf(stderr, "Could not allocate %d bytes for code",
                sizeof(uint8_t) * size);
        fclose(f);
        return 1;
    }

    size_t bytes_read = fread((void *)code, sizeof(uint8_t), size, f);
    fclose(f);
    if (bytes_read != size) {
        fprintf(
            stderr,
            "Code could not be read fully: size = %d bytes, read = %d bytes",
            size, bytes_read);
        free(code);
        return 1;
    }

    void *entrypoints[] = {(void *)code, NULL};

    BlockList *blocks = discover_blocks(entrypoints);
    void *code_base = code;
    free(code);

    if (blocks == NULL) {
        fprintf(stderr, "collect_blocks() returned NULL");
        return 1;
    }

    size_t block_count = 0;
    for (BlockList *curr = blocks; curr != NULL; curr = curr->next) {
        block_count++;
    }
    PRINT_DEBUG("Blocks found:\t%d", block_count);

    f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "Could not open function map file '%s'", argv[2]);
        free_block_list(blocks);
        return 1;
    }

    char line[256];
    size_t functions_expected = 0;
    size_t functions_found = 0;
    while (fgets(line, 256, f) != NULL) {
        uint32_t addr;
        char name[64];
        sscanf(line, "%X %64s", &addr, name);
        bool function_found = false;
        for (BlockList *curr = blocks; curr != NULL; curr = curr->next) {
            Block *ble = curr->block;
            if (BYTE_DIFF(ble->start, code_base) == addr) {
                function_found = true;
                break;
            }
        }
        functions_expected++;
        if (function_found) {
            // PRINT_DEBUG("Found function: %s", name);
            functions_found++;
        } else {
            if (is_blacklisted(&name)) {
                functions_expected--;
            } else {
                PRINT_DEBUG("Function not found: %s", name);
            }
        }
    }
    fclose(f);

    PRINT_DEBUG("Found %d/%d functions", functions_found, functions_expected);

    free_block_list(blocks);
    return functions_found != functions_expected;
}
