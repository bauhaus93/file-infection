#include "pe.h"
#include "process_info.h"
#include "string_generator.h"
#include "utility.h"

uint8_t is_pe(void *base) {
    return *(uint16_t *)base == 0x5A4D &&
           get_nt_header_by_base(base)->OptionalHeader.Magic ==
               (IS_32_BIT ? IMAGE_NT_OPTIONAL_HDR32_MAGIC
                          : IMAGE_NT_OPTIONAL_HDR64_MAGIC);
}

uint32_t align_value(uint32_t value, uint32_t alignment) {
    return (value + alignment) - (value % alignment);
}

IMAGE_NT_HEADERS *get_nt_header_by_base(void *base) {
    if (base == NULL) {
        return NULL;
    }
    IMAGE_DOS_HEADER *dosHdr = (IMAGE_DOS_HEADER *)base;
    return (IMAGE_NT_HEADERS *)((uint8_t *)base + dosHdr->e_lfanew);
}

IMAGE_NT_HEADERS *get_nt_header(void) {
    return get_nt_header_by_base(get_image_base());
}

IMAGE_EXPORT_DIRECTORY *get_export_directory_by_base(void *base) {
    if (base == NULL) {
        return NULL;
    }
    IMAGE_NT_HEADERS *nt_header = get_nt_header_by_base(base);
    return (IMAGE_EXPORT_DIRECTORY *)((uint8_t *)base +
                                      nt_header->OptionalHeader.DataDirectory[0]
                                          .VirtualAddress);
}

IMAGE_EXPORT_DIRECTORY *get_export_directory(void) {
    return get_export_directory_by_base(get_image_base());
}

IMAGE_EXPORT_DIRECTORY *get_kernel32_export_directory(void) {
    return get_export_directory_by_base(get_kernel32_base());
}

IMAGE_SECTION_HEADER *get_section_header(IMAGE_NT_HEADERS *nt_headers,
                                         uint16_t index) {
    IMAGE_SECTION_HEADER *section_header =
        (IMAGE_SECTION_HEADER
             *)((uint8_t *)nt_headers +
                sizeof(IMAGE_NT_HEADERS)); // was nt_headers + 1???
    return section_header + index;
}

IMAGE_SECTION_HEADER *get_last_section_header(IMAGE_NT_HEADERS *nt_headers) {
    return get_section_header(nt_headers,
                              nt_headers->FileHeader.NumberOfSections - 1);
}

uint8_t is_section_header_empty(IMAGE_SECTION_HEADER *section_header) {
    for (uint32_t *ptr = (uint32_t *)section_header;
         (IMAGE_SECTION_HEADER *)ptr < section_header + 1; ptr++) {
        if (*ptr != 0)
            return 0;
    }
    return 1;
}

void create_section_header(IMAGE_SECTION_HEADER *section_header,
                           IMAGE_NT_HEADERS *nt_headers, uint32_t code_size) {
    nt_headers->FileHeader.NumberOfSections++;

    if (get_string(STRING_SECTION_NAME, (char *)section_header->Name,
                   IMAGE_SIZEOF_SHORT_NAME) == 0) {
        PRINT_DEBUG("could not get string for STRING_SECTION_NAME");
    }

    section_header->Misc.VirtualSize = code_size;
    section_header->VirtualAddress =
        (section_header - 1)->VirtualAddress +
        align_value((section_header - 1)->Misc.VirtualSize,
                    nt_headers->OptionalHeader.SectionAlignment);
    section_header->SizeOfRawData =
        align_value(code_size, nt_headers->OptionalHeader.FileAlignment);
    section_header->PointerToRawData =
        (section_header - 1)->PointerToRawData +
        align_value((section_header - 1)->SizeOfRawData,
                    nt_headers->OptionalHeader.FileAlignment);
    section_header->Characteristics =
        IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
}
