#include "virus.h"


void start_code(void) {}

void memdump(uint8_t *start, uint8_t *end) {
  printf("memdump:");
  uint32_t i = 0;
  while (start <= end) {
    if ((uint32_t)i % 16 == 0)
      printf("\n0x%X | ", start);
    else if ((uint32_t)i % 8 == 0)
      printf(" ");
    printf("%02X ", *start);
    start++;
    i++;
  }
  printf("\n");
}

uint8_t* get_nt_header(uint8_t *base) {
  return base + *(base + 0x3C);
}

int run(void) {
  Data data;
  IMAGE_NT_HEADERS *ntHdr = NULL;
  uint8_t *ptr = NULL;

  printf("current code size: %d bytes\n", end_code - start_code);

  data.imageBase = get_image_base();
  data.kernel32Base = get_kernel32_base();

  if (*(uint16_t*)data.imageBase != 0x5A4D)  //"MZ"
    return 1;

  if (*(uint16_t*)data.kernel32Base != 0x5A4D)  //"MZ"
    return 2;

  printf("image base: 0x%X\nkernel32 base: 0x%X\n", data.imageBase, data.kernel32Base);

  ntHdr = get_nt_header(data.kernel32Base);


  IMAGE_EXPORT_DIRECTORY *exportDir = data.kernel32Base + ntHdr->OptionalHeader.DataDirectory[0].VirtualAddress;

  memdump(exportDir, exportDir + sizeof(IMAGE_EXPORT_DIRECTORY));

  printf("%s\n", data.kernel32Base + exportDir->Name);

  uint32_t* namePtr = data.kernel32Base + exportDir->AddressOfNames;
  uint32_t* addrPtr = data.kernel32Base + exportDir->AddressOfFunctions;
  for(uint32_t i = 0; i < exportDir->NumberOfNames; i++) {
    printf("%s\n", data.kernel32Base + *namePtr);

    namePtr++;
  }

  return 0;
}
