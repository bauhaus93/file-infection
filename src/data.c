#include <windows.h>

#include "data.h"
#include "pe.h"
#include "process_info.h"
#include "utility.h"

#define DATA_MARKER ((data_t *)-1)

static data_t *__attribute__((noinline)) get_data_pointer(void);
static void load_data_pointer(void);
static int fill_data(data_t *data);

data_t *get_data(void) {
    if (!data_initialized()) {
        load_data_pointer();
        if (!data_initialized()) {
            PRINT_DEBUG("could not set data pointer");
            return NULL;
        }
        PRINT_DEBUG("loaded data pointer");
    }
    return get_data_pointer();
}

uint8_t data_initialized(void) { return get_data_pointer() != DATA_MARKER; }

void free_data(void) {
    if (get_data_pointer() != DATA_MARKER) {
        data_t *ptr = get_data_pointer();
        if (ptr->function_list.virtual_free((LPVOID)get_data_pointer(),
                                            sizeof(data_t), MEM_DECOMMIT)) {
            PRINT_DEBUG("freed data in data pointer");
        } else {
            PRINT_DEBUG("could not free data in data pointer");
        }
    }
}
void reset_data_pointer(void *target_code_begin) {
    if (find_replace_value32((uint32_t)get_data_pointer(),
                             (uint32_t)DATA_MARKER, get_data_pointer,
                             load_data_pointer, target_code_begin) != 0) {
        PRINT_DEBUG("could not reset data pointer");
    }
}

#pragma GCC push_options
#pragma GCC optimize("O0")
static data_t *__attribute__((noinline)) get_data_pointer(void) {
    return DATA_MARKER;
}
#pragma GCC pop_options
// no functions between get_data_pointer and set_data_pointer!

// no functions between get_data_pointer and set_data_pointer!

static void load_data_pointer(void) {
    data_t data_local;
    memzero(&data_local, sizeof(data_t));

    if (fill_data(&data_local) != 0) {
        PRINT_DEBUG("could not init local data");
        return;
    }

    data_t *data_ptr = data_local.function_list.virtual_alloc(
        NULL, sizeof(data_t), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (data_ptr == NULL) {
        PRINT_DEBUG("could not allocate memory for data");
        return;
    }
    memcp(&data_local, data_ptr, sizeof(data_t));

    uint32_t *data_marker = (uint32_t *)find_value32(
        (uint32_t)DATA_MARKER,
        BYTE_OFFSET(get_data_pointer, data_ptr->delta_offset),
        BYTE_OFFSET(load_data_pointer, data_ptr->delta_offset));

    if (data_marker != NULL) {
        DWORD old_protect;

        if (!data_ptr->function_list.virtual_protect(
                (LPVOID)data_marker, (SIZE_T)sizeof(uint32_t),
                PAGE_EXECUTE_READWRITE, &old_protect)) {
            PRINT_DEBUG("could not change access rights");
            return;
        }

        *(data_t **)data_marker = data_ptr;

        if (!data_ptr->function_list.virtual_protect(
                (LPVOID)data_marker, (SIZE_T)sizeof(uint32_t), old_protect,
                &old_protect)) {
            PRINT_DEBUG("could not restore access rights");
        }
    } else {
        PRINT_DEBUG("could not locate data marker");
    }
}

static int fill_data(data_t *data) {
    memzero(data, sizeof(data_t));
    data->delta_offset = get_delta_offset();
    data->image_base = get_image_base();
    data->kernel32_base = get_kernel32_base();

    if (data->image_base == NULL || !is_pe(data->image_base)) {
        PRINT_DEBUG("invalid image_base");
        return 1;
    }
    PRINT_DEBUG("image_base: 0x%p", data->image_base);
    if (data->kernel32_base == NULL || !is_pe(data->kernel32_base)) {
        PRINT_DEBUG("invalid kernel32_base");
        return 2;
    }
    PRINT_DEBUG("kernel32_base: 0x%p", data->kernel32_base);

    IMAGE_NT_HEADERS *nt_header = get_nt_header(data->kernel32_base);
    IMAGE_EXPORT_DIRECTORY *export_dir =
        get_export_directory(data->kernel32_base, nt_header);

    if (fill_function_list(export_dir, data->kernel32_base,
                           &data->function_list) !=
        sizeof(function_list_t) / sizeof(fpExitProcess *)) {
        return 3;
    }
    return 0;
}
