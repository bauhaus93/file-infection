#include <stdio.h>
#include <stdlib.h>

#include "disasm.h"

#define CHECK_INSTRUCTION(expected, found, count)                              \
    {                                                                          \
        if (expected != found) {                                               \
            fprintf(stderr,                                                    \
                    "Expected instruction #%d of size %d, but found %d",       \
                    count, expected, found);                                   \
            exit(1);                                                           \
        }                                                                      \
    }

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(
            stderr,
            "Usage: check_instruction_length expected_size instruction_file\n");
        return 1;
    }
    uint8_t expected_size = (uint8_t)atoi(argv[1]);
    if (expected_size == 0) {
        fprintf(stderr,
                "Invalid expected_size given: Cannot be zero, but was zero\n");
        return 1;
    }

    const char *filename = argv[2];
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "Could not open file '%s'", filename);
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    void *buffer = malloc(sizeof(uint8_t) * size);
    if (buffer == NULL) {
        fprintf(stderr,
                "Could not allocate buffer of size %d for file buffer\n", size);
        return 1;
    }

    size_t bytes_read = fread(buffer, sizeof(uint8_t), size, f);
    fclose(f);
    if (bytes_read != size) {
        fprintf(stderr, "Couldn't read whole file, %d/%d bytes", bytes_read,
                size);
        return 1;
    }

    int instruction_counter = 0;
    void *ptr = buffer;
    void *end = (void *)(((uint8_t *)ptr) + sizeof(uint8_t) * size);
    Instruction * instr = parse_instruction(buffer);
    if (instr == NULL) {
        fprintf(stderr, "Could not parse instruction #%d\n",
                instruction_counter);
        return 1;
    }
    while (instr->addr < end) {
        uint8_t found_size = get_instruction_size(instr);
        CHECK_INSTRUCTION(expected_size, found_size, instruction_counter);
        instruction_counter++;
        Instruction * next_instr = next_instruction(instr);
        FREE(instr);
        instr = next_instr;
        if (instr == NULL) {
            fprintf(stderr, "Could not parse instruction #%d\n",
                    instruction_counter);
            return 1;
        }
    }

    return 0;
}
