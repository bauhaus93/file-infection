#ifndef UTILITY_H
#define UTILITY_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "functions.h"
#include "checksum.h"
#include "checksum_list.h"

#define DEBUGLEL

#ifdef DEBUGLEL
  #define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
  #define PRINT_DEBUG(...)
#endif

#define IS_PE(baseAddr) (*(WORD*)baseAddr == 0x5A4D && get_nt_header(baseAddr)->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC)
#define ALIGN_VALUE(value, alignment) (value + alignment - (value % alignment))

IMAGE_NT_HEADERS* get_nt_header(void* base);
IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr);

void* get_address_by_checksum(IMAGE_EXPORT_DIRECTORY* ed, void* base, uint32_t cs);
int fill_addresses(IMAGE_EXPORT_DIRECTORY* ed, void* base, functions_t* functions);

int is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader);

void memzero(void* start, uint32_t size);
void memcp(void* src, void* dest, uint32_t size);

#endif
