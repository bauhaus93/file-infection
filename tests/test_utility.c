#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
    fprintf(stderr, "Could not allocate %d bytes for code\n",
            sizeof(uint8_t) * (*size));
    fclose(f);
    return NULL;
  }

  size_t bytes_read = fread((void *)code, sizeof(uint8_t), *size, f);
  fclose(f);
  if (bytes_read != *size) {
    fprintf(stderr,
            "Code could not be read fully: size = %d bytes, read = %d bytes\n",
            *size, bytes_read);
    free(code);
    return NULL;
  }
  return code;
}
