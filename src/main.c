#include <stdio.h>
#include <windows.h>
#include <stdint.h>

#include "ass.h"


int main(int argc, char **argv) {
  printf("image_base:\t0x%X\n", get_image_base());
  printf("kernel32_base:\t0x%X\n", get_kernel32_base());

  uint8_t *img = get_image_base();
  uint8_t *kernel32 = get_kernel32_base();

  printf("img: %c%c\n", img[0], img[1]);
  printf("kernel32: %c%c\n", img[0], img[1]);

  return 0;
}
