#ifndef PE_H
#define PE_H

#include <stdint.h>
#include <windows.h>

uint8_t is_pe(void *base_addr);
uint32_t align_value(uint32_t value, uint32_t alignment);

IMAGE_NT_HEADERS *get_nt_header(void *base);
IMAGE_SECTION_HEADER *get_section_header(IMAGE_NT_HEADERS *nt_headers,
                                         uint16_t index);
IMAGE_SECTION_HEADER *get_last_section_header(IMAGE_NT_HEADERS *nt_headers);
IMAGE_EXPORT_DIRECTORY *get_export_directory(void *base,
                                             IMAGE_NT_HEADERS *nt_header);
uint8_t is_section_header_empty(IMAGE_SECTION_HEADER *section_header);
void create_section_header(IMAGE_SECTION_HEADER *section_header,
                           IMAGE_NT_HEADERS *nt_headers, uint32_t code_size);

#endif
