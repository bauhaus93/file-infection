#include "virus.h"


void start_code(void) {}

void memdump(uint8_t *start, uint8_t *end) {
  printf("memdump:");
  while (start <= end) {
    if ((uint32_t)start % 16 == 0)
      printf("\n0x%X | ", start);
    else if ((uint32_t)start % 8 == 0)
      printf(" ");
    printf("%02X ", *start);
    start++;
  }
  printf("\n");
}

uint8_t* get_nt_header(uint8_t *base) {
  return base + *(base + 0x3C);
}

uint8_t* get_optional_header(uint8_t *base) {
  return get_nt_header(base) + 0x18;
}

int check_pe_constants(uint8_t *base) {
  if (*(uint16_t*)base != 0x5A4D)  //"MZ"
    return 1;

  base += *(base + 0x3C);

  if (*(uint16_t*)base != 0x4550)  //"PE"
    return 2;

  base += 0x18;

  if (*(uint16_t*)base != 0x10B)   //Magic
    return 3;

  return 0;
}

int run(void) {
  Data data;
  uint8_t *ptr = NULL;

  printf("current code size: %d bytes\n", end_code - start_code);

  data.imageBase = get_image_base();
  data.kernel32Base = get_kernel32_base();

  printf("image base: 0x%X\nkernel32 base: 0x%X\n", data.imageBase, data.kernel32Base);

  memdump(data.kernel32Base, data.kernel32Base + 0x100);

  int err = check_pe_constants(data.kernel32Base);
  if (err != 0) {
    printf("Not all PE constants found, ret val = %d\n", err);
  }

  uint8_t *ntHeader = get_nt_header(data.kernel32Base);

  ptr = ntHeader + 0x78;    //

  printf("ptr @ 0x%X = 0x%X\n", ntHeader, *(uint16_t*)ntHeader);

  return 0;
}
