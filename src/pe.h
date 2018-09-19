#ifndef PE_H
#define PE_H

#include <windows.h>
#include <stdint.h>

#include "data.h"

uint8_t is_pe(void* baseAddr);
uint32_t align_value(uint32_t value, uint32_t alignment);

IMAGE_NT_HEADERS* get_nt_header(void* base);
IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr);
uint8_t is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader);
void create_section_header(IMAGE_SECTION_HEADER* sectionHeader, IMAGE_NT_HEADERS* ntHeaders, uint32_t codeSize);

#endif