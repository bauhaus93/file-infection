#include "pe.h"

uint8_t is_pe(void* baseAddr) {
  return  *(uint16_t*)baseAddr == 0x5A4D &&
          get_nt_header(baseAddr)->OptionalHeader.Magic == (IS_32_BIT ? IMAGE_NT_OPTIONAL_HDR32_MAGIC : IMAGE_NT_OPTIONAL_HDR64_MAGIC);
}

uint32_t align_value(uint32_t value, uint32_t alignment) {
  return (value + alignment) - (value % alignment);
}

IMAGE_NT_HEADERS* get_nt_header(void* base) {
  IMAGE_DOS_HEADER* dosHdr = (IMAGE_DOS_HEADER*)base;
  return (IMAGE_NT_HEADERS*) ((uint8_t*)base + dosHdr->e_lfanew);
}

IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr) {
    return (IMAGE_EXPORT_DIRECTORY*) ((uint8_t*)base + ntHdr->OptionalHeader.DataDirectory[0].VirtualAddress);
}

IMAGE_SECTION_HEADER* get_section_header(IMAGE_NT_HEADERS* ntHeaders, uint16_t index) {
  IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*) ((uint8_t*)ntHeaders + sizeof(IMAGE_NT_HEADERS)); //was ntHeaders + 1???
  return sectionHeader + index;
}

IMAGE_SECTION_HEADER* get_last_section_header(IMAGE_NT_HEADERS* ntHeaders) {
  return get_section_header(ntHeaders, ntHeaders->FileHeader.NumberOfSections - 1);
}

uint8_t is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader) {
  for(uint32_t *ptr = (uint32_t*)sectionHeader; (IMAGE_SECTION_HEADER*)ptr < sectionHeader + 1; ptr++) {
    if(*ptr != 0)
      return 0;
  }
  return 1;
}

void create_section_header(IMAGE_SECTION_HEADER* sectionHeader, IMAGE_NT_HEADERS* ntHeaders, uint32_t codeSize) {
  ntHeaders->FileHeader.NumberOfSections++;

  sectionHeader->Name[0] = '.';
  sectionHeader->Name[1] = 'v';
  sectionHeader->Name[2] = 'i';
  sectionHeader->Name[3] = 'r';
  sectionHeader->Name[4] = 'u';
  sectionHeader->Name[5] = 's';

  sectionHeader->Misc.VirtualSize = codeSize;
  sectionHeader->VirtualAddress = (sectionHeader - 1)->VirtualAddress + align_value((sectionHeader - 1)->Misc.VirtualSize, ntHeaders->OptionalHeader.SectionAlignment);
  sectionHeader->SizeOfRawData = align_value(codeSize, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->PointerToRawData = (sectionHeader - 1)->PointerToRawData + align_value((sectionHeader - 1)->SizeOfRawData, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
}