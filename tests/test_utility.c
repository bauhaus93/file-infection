#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/block/copy.h"
#include "../src/block/discovery.h"
#include "../src/utility.h"

void *read_binary(const char *path, size_t *size) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    fprintf(stderr, "Could not open file '%s'\n", path);
    *size = 0;
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  fseek(f, 0, SEEK_SET);

  uint8_t *code = (uint8_t *)malloc(sizeof(uint8_t) * (*size));
  if (code == NULL) {
    fprintf(stderr, "Could not allocate %lu bytes for code\n",
            sizeof(uint8_t) * (*size));
    fclose(f);
    return NULL;
  }

  size_t bytes_read = fread((void *)code, sizeof(uint8_t), *size, f);
  fclose(f);
  if (bytes_read != *size) {
    fprintf(
        stderr,
        "Code could not be read fully: size = %lu bytes, read = %lu bytes\n",
        *size, bytes_read);
    free(code);
    return NULL;
  }
  return code;
}

void *copy_code(void *entry_point, size_t *target_size) {
  void *entrypoints[] = {entry_point, NULL};

  BlockList *blocks = discover_blocks(entrypoints);

  if (blocks == NULL) {
    fprintf(stderr, "Could not discover blocks\n");
    return NULL;
  }

  *target_size = estimate_target_code_size(blocks);
  assert(*target_size > 0);
  PRINT_DEBUG("Target code size estimate = %.1fkB",
              (float)*target_size / 1024.);

  PRINT_DEBUG("Allocating target memory");
  void *target_mem = malloc(*target_size);
  assert(target_mem != NULL);
  PRINT_DEBUG("Target base is %p", target_mem);

  if (!copy_blocks(blocks, target_mem, *target_size)) {
    fprintf(stderr, "Could not copy modified blocks\n");
    return NULL;
  }
  void *target_entrypoint = get_target_address(entrypoints[0], blocks);
  free_block_list(blocks);
  PRINT_DEBUG("Target entry point is %p", target_entrypoint);
  return target_mem;
}
