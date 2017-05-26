#ifndef UTILITY_H
#define UTILITY_H

#include <windows.h>
#include <stdint.h>

#include "functions.h"
#include "checksum.h"
#include "checksum_list.h"

#define VERIFY_MZ(addr) (*((uint16_t*)addr) == 0x5A4D)
#define VERIFY_MAGIC(addr) (*((uint16_t*)addr) ==  IMAGE_NT_OPTIONAL_HDR_MAGIC)
#define ALIGN_VALUE(value, alignment) (value + alignment - (value % alignment))

IMAGE_NT_HEADERS* get_nt_header(void* base);
IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr);

void* get_address_by_checksum(IMAGE_EXPORT_DIRECTORY* ed, void* base, uint32_t cs);
int fill_addresses(IMAGE_EXPORT_DIRECTORY* ed, void* base, functions_t* functions);

int is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader);

void memzero(void* start, uint32_t size);

#endif
