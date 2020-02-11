#include "data.h"
#include "pe.h"
#include "utility.h"
#include "process_info.h"

int init_data(data_t *data, void *codeBegin, void *codeEnd, void *entryPoint) {

    memzero(data, sizeof(data_t));
    data->deltaOffset = get_delta_offset();
    data->codeBegin = (void *)((uint8_t *)codeBegin + data->deltaOffset);
    data->codeEnd = (void *)((uint8_t *)codeEnd + data->deltaOffset);
    data->codeSize = (uint8_t *)data->codeEnd - (uint8_t *)data->codeBegin;
    data->entryOffset = (uint8_t *)entryPoint - (uint8_t *)codeBegin;

    data->imageBase = get_image_base();
    data->kernel32Base = get_kernel32_base();

    if (!is_pe(data->imageBase)) {
        return 1;
    }
    if (!is_pe(data->kernel32Base)) {
        return 2;
    }

    IMAGE_NT_HEADERS *ntHdr = get_nt_header(data->kernel32Base);
    IMAGE_EXPORT_DIRECTORY *exportDir =
        get_export_directory(data->kernel32Base, ntHdr);

    if (fill_functions(exportDir, data->kernel32Base, &data->functions) !=
        sizeof(functions_t) / sizeof(fpExitProcess *)) {
        return 3;
    }
    return 0;
}
