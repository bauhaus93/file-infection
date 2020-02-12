#include "data.h"
#include "pe.h"
#include "utility.h"
#include "process_info.h"

int init_data(data_t *data, void *code_begin_addr, void *code_end_addr, void *entry_point) {
    memzero(data, sizeof(data_t));
    data->delta_offset = get_delta_offset();
    data->code_begin_addr = (void *)((uint8_t *)code_begin_addr + data->delta_offset);
    data->code_end_addr = (void *)((uint8_t *)code_end_addr + data->delta_offset);
    data->code_size = (uint8_t *)data->code_end_addr - (uint8_t *)data->code_begin_addr;
    data->entry_offset = (uint8_t *)entry_point - (uint8_t *)code_begin_addr;

    data->image_base = get_image_base();
    data->kernel32_base = get_kernel32_base();


    if (data->image_base == NULL || !is_pe(data->image_base)) {
        PRINT_DEBUG("invalid image_base");
        return 1;
    }
    if (data->kernel32_base == NULL || !is_pe(data->kernel32_base)) {
        PRINT_DEBUG("invalid kernel32_base");
        return 2;
    }

    IMAGE_NT_HEADERS *nt_header = get_nt_header(data->kernel32_base);
    IMAGE_EXPORT_DIRECTORY *export_dir =
        get_export_directory(data->kernel32_base, nt_header);

    if (fill_function_list(export_dir, data->kernel32_base, &data->function_list) !=
        sizeof(function_list_t) / sizeof(fpExitProcess *)) {
        return 3;
    }
    return 0;
}
