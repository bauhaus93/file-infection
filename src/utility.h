#ifndef UTILITY_H
#define UTILITY_H

#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#include "functions.h"
#include "checksum.h"
#include "checksum_list.h"


#ifdef NDEBUG
#define PRINT_DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define PRINT_DEBUG(...)
#endif

int32_t get_delta_offset(void);
uint8_t is_pe(void* baseAddr);
uint32_t align_value(uint32_t value, uint32_t alignment);

IMAGE_NT_HEADERS* get_nt_header(void* base);
IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr);

void* get_address_by_checksum(IMAGE_EXPORT_DIRECTORY* ed, void* base, uint32_t cs);
int fill_addresses(IMAGE_EXPORT_DIRECTORY* ed, void* base, functions_t* functions);

int is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader);

void memzero(void* start, uint32_t size);
void memcp(void* src, void* dest, uint32_t size);

#endif
