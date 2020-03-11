#ifndef FUNCTION_KERNEL32_H
#define FUNCTION_KERNEL32_H

#include <stdint.h>
#include <windows.h>

#include "checksum_list.h"

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

void *get_kernel32_function_pointer(uint32_t function_checksum);

#define CALL_KERNEL32_FUNCTION(fp, cs, ...)                                    \
    (((fp)(get_kernel32_function_pointer(cs)))(__VA_ARGS__))



#define VIRTUAL_ALLOC(...)                                                      \
    (CALL_KERNEL32_FUNCTION(fpVirualAlloc, CS_VIRTUALALLOC, __VA_ARGS__))
#define VIRTUAL_FREE(...)                                                      \
    (CALL_KERNEL32_FUNCTION(fpVirtualFree, CS_VIRTUALFREE, __VA_ARGS__))

#define EXIT_PROCESS(...)                                                      \
    (CALL_KERNEL32_FUNCTION(fpExitProcess, CS_EXITPROCESS, __VA_ARGS__))

#define CLOSE_HANDLE(...)                                                      \
    (CALL_KERNEL32_FUNCTION(fpCloseHandle, CS_CLOSEHANDLE, __VA_ARGS__))

#define CREATE_FILE_A(...)                                                     \
    (CALL_KERNEL32_FUNCTION(fpCreateFileA, CS_CREATEFILEA, __VA_ARGS__))
#define GET_FILE_SIZE(...)                                                     \
    (CALL_KERNEL32_FUNCTION(fpGetFileSize, CS_GETFILESIZE, __VA_ARGS__))
#define CREATE_FILE_MAPPING_A(...)                                             \
    (CALL_KERNEL32_FUNCTION(fpCreateFileMappingA, CS_CREATEFILEMAPPINGA,       \
                            __VA_ARGS__))
#define MAP_VIEW_OF_FILE(...)                                                  \
    (CALL_KERNEL32_FUNCTION(fpMapViewOfFile, CS_MAPVIEWOFFILE, __VA_ARGS__))
#define UNMAP_VIEW_OF_FILE(...)                                                \
    (CALL_KERNEL32_FUNCTION(fpUnmapViewOfFile, CS_UNMAPVIEWOFFILE, __VA_ARGS__))
#define FLUSH_VIEW_OF_FILE(...)                                                \
    (CALL_KERNEL32_FUNCTION(fpFlushViewOfFile, CS_FLUSHVIEWOFFILE, __VA_ARGS__))

#define CREATE_THREAD(...)                                                     \
    (CALL_KERNEL32_FUNCTION(fpCreateThread, CS_CREATETHREAD, __VA_ARGS__))
#define FIND_FIRST_FILE_A(...)                                                 \
    (CALL_KERNEL32_FUNCTION(fpFindFirstFileA, CS_FINDFIRSTFILEA, __VA_ARGS__))
#define FIND_NEXT_FILE_A(...)                                                  \
    (CALL_KERNEL32_FUNCTION(fpFindNextFileA, CS_FINDNEXTFILEA, __VA_ARGS__))
#define FIND_CLOSE(...)                                                        \
    (CALL_KERNEL32_FUNCTION(fpFindClose, CS_FINDCLOSE, __VA_ARGS__))

#endif // FUNCTION_KERNEL32_H 
