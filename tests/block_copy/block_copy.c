#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../src/block/copy.h"
#include "../../src/block/discovery.h"
#include "../../src/utility.h"
#include "../test_utility.h"

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
  void *code = read_binary(src_binary, &size);
  if (code == NULL) {
    fprintf(stderr, "Could not read binary\n");
    return false;
  }
  PRINT_DEBUG("Size:\t\t%.1f kiB", ((float)size) / 1024.0);

  size_t target_size = 0;
  void *target_code = copy_code(code, &target_size);
  memdump(target_code, target_size, target_binary);

  free(target_code);
  return true;
}

int main(int argc, char **argv) {

  if (argc != 3) {
    fprintf(stderr, "Need 2 args: Path of code binary dump, followed by output "
                    "path of modified binary");
    return 1;
  }

  if (!write_modified_code_copy(argv[1], argv[2])) {
    fprintf(stderr, "Could not write modified code copy");
    return 1;
  }

  return 0;
}
