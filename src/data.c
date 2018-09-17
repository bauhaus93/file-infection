#include "data.h"

int init_data(data_t* data, void* codeBegin, void* codeEnd, uint32_t entryOffset) {
  memzero(data, sizeof(data_t));

  PRINT_DEBUG("entry offset = 0x%X\n", entryOffset);

  data->deltaOffset = get_delta_offset();
  data->codeBegin = (void*)((uint8_t*)codeBegin + data->deltaOffset);
  data->codeEnd = (void*)((uint8_t*)codeEnd + data->deltaOffset);
  data->codeSize = (uint8_t*)data->codeEnd - (uint8_t*)data->codeBegin;
  data->entryOffset = entryOffset;

  data->imageBase = get_image_base();
  data->kernel32Base = get_kernel32_base();

  assert(is_pe(data->imageBase));
  assert(is_pe(data->kernel32Base));
  assert(data->entryOffset < data->codeSize);

  IMAGE_NT_HEADERS* ntHdr = get_nt_header(data->kernel32Base);
  IMAGE_EXPORT_DIRECTORY* exportDir = get_export_directory(data->kernel32Base, ntHdr);

  if(fill_addresses(exportDir, data->kernel32Base, &data->functions) != sizeof(functions_t) / sizeof(fpExitProcess*)) {
    return 3;
  }

  return 0;

}