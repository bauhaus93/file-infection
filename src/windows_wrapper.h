#ifndef WINDOWS_WRAPPER_H
#define WINDOWS_WRAPPER_H

#ifndef WINDOWS_HEADER_DUMMY

#include <windows.h>

#else

// #warning "Building with dummy declarations of windows.h definitions, which only should allow compilation.

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef void* HANDLE;
typedef uint32_t DWORD;
typedef bool BOOL;
typedef long LONG, *PLONG;
typedef size_t SIZE_T;
typedef unsigned int UINT;
typedef DWORD *PDWORD, *LPDWORD;
typedef const char * LPCTSTR;
typedef void VOID, *LPVOID;
typedef const LPVOID LPCVOID;
typedef void * LPTHREAD_START_ROUTINE;

typedef struct {
	DWORD TimeDateStamp;	
	DWORD NumberOfSections;
}IMAGE_FILE_HEADER;

typedef struct {
	uint16_t Magic;
	uint32_t ImageBase;
	uint32_t AddressOfEntryPoint;
	uint32_t FileAlignment;
	uint32_t SectionAlignment;
	uint32_t SizeOfHeaders;
	uint32_t SizeOfImage;
	uint32_t BaseOfCode;
	uint32_t SizeOfCode;
	struct {
		DWORD VirtualAddress;
	}DataDirectory[16];
}IMAGE_OPTIONAL_HEADER;

typedef struct {
	uint16_t e_lfanew;
} IMAGE_DOS_HEADER;

typedef struct {
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS;

typedef struct {
	LPCTSTR Name;
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t Characteristics;
	DWORD PointerToRawData;
	struct {
		DWORD VirtualSize;
	}Misc;
} IMAGE_SECTION_HEADER;

typedef struct {
	DWORD NumberOfNames;
	DWORD AddressOfNames;
	DWORD AddressOfNameOrdinals;
	DWORD AddressOfFunctions;
} IMAGE_EXPORT_DIRECTORY;

typedef struct {
	const char * cFileName;
} WIN32_FIND_DATA, WIN32_FIND_DATAA, *LPWIN32_FIND_DATA;

typedef struct {
} *LPSECURITY_ATTRIBUTES;

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0
#define IMAGE_NT_OPTIONAL_HDR64_MAGIC 0

#define WINAPI
#define MEM_RELEASE 0
#define MEM_COMMIT 0
#define PAGE_READWRITE 0
#define IMAGE_SCN_MEM_READ 0
#define IMAGE_SCN_MEM_EXECUTE 0
#define IMAGE_SCN_MEM_WRITE 0
#define IMAGE_SCN_CNT_CODE 0
#define IMAGE_SIZEOF_SHORT_NAME 0
#define INVALID_HANDLE_VALUE 0
#define INVALID_FILE_SIZE 0
#define FILE_ATTRIBUTE_NORMAL 0
#define FILE_SHARE_WRITE 0
#define FILE_SHARE_READ 0
#define FILE_MAP_ALL_ACCESS 0
#define GENERIC_READ 0
#define GENERIC_WRITE 0
#define OPEN_EXISTING 0

#endif

#endif // WINDOWS_WRAPPER_H
