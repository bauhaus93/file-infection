#include "pe.h"

uint8_t is_pe(void* baseAddr) {
  return  *(uint16_t*)baseAddr == 0x5A4D &&
          get_nt_header(baseAddr)->OptionalHeader.Magic == (IS_32_BIT ? IMAGE_NT_OPTIONAL_HDR32_MAGIC : IMAGE_NT_OPTIONAL_HDR64_MAGIC);
}

uint32_t align_value(uint32_t value, uint32_t alignment) {
  return value + alignment - (value % alignment);
}

IMAGE_NT_HEADERS* get_nt_header(void* base) {
  IMAGE_DOS_HEADER* dosHdr = (IMAGE_DOS_HEADER*)base;
  return (IMAGE_NT_HEADERS*) ((uint8_t*)base + dosHdr->e_lfanew);
}

IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr) {
    return (IMAGE_EXPORT_DIRECTORY*) ((uint8_t*)base + ntHdr->OptionalHeader.DataDirectory[0].VirtualAddress);
}

uint8_t is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader) {
  for(uint32_t *ptr = (uint32_t*)sectionHeader; (IMAGE_SECTION_HEADER*)ptr < sectionHeader + 1; ptr++) {
    if(*ptr != 0)
      return 0;
  }
  return 1;
}

