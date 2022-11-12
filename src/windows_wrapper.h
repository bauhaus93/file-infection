#ifndef WINDOWS_WRAPPER_H
#define WINDOWS_WRAPPER_H

// #warning "Building with dummy declarations of windows.h definitions, which
// only should allow compilation.

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint32_t DWORD, *PDWORD, *LPDWORD;
typedef uint16_t WORD;
typedef bool BOOL;
typedef long LONG, *PLONG;
typedef size_t SIZE_T;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef const char *LPCTSTR;
typedef void VOID, *PVOID, *LPVOID;
typedef const LPVOID LPCVOID;
typedef void *LPTHREAD_START_ROUTINE;
typedef PVOID HANDLE;

#define IMAGE_NT_OPTIONAL_HDR32_MAGIC 0x10b
#define IMAGE_NT_OPTIONAL_HDR_MAGIC IMAGE_NT_OPTIONAL_HDR32_MAGIC
#define IMAGE_SIZEOF_SHORT_NAME 8
#define IMAGE_SIZEOF_SECTION_HEADER 40
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION 3
#define IMAGE_DIRECTORY_ENTRY_SECURITY 4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5
#define IMAGE_DIRECTORY_ENTRY_DEBUG 6
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE 7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 8
#define IMAGE_DIRECTORY_ENTRY_TLS 9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 11
#define IMAGE_DIRECTORY_ENTRY_IAT 12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT 13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14

#define WINAPI __attribute__((fastcall))
#define MEM_RELEASE (0x00008000)
#define MEM_COMMIT (0x00001000)
#define MEM_RESERVE (0x00002000)
#define PAGE_READWRITE (0x04)
#define IMAGE_SCN_MEM_READ (0x40000000)
#define IMAGE_SCN_MEM_EXECUTE (0x20000000)
#define IMAGE_SCN_MEM_WRITE (0x80000000)
#define IMAGE_SCN_CNT_CODE (0x00000020)
#define INVALID_HANDLE_VALUE ((HANDLE)-1)
#define INVALID_FILE_SIZE (-1)
#define FILE_ATTRIBUTE_NORMAL (0x80)
#define FILE_SHARE_WRITE (0x00000002)
#define FILE_SHARE_READ (0x00000001)
#define MAX_PATH (260)

#define STANDARD_RIGHTS_REQUIRED (0x000F0000)
#define SECTION_QUERY (0x0001)
#define SECTION_MAP_WRITE (0x0002)
#define SECTION_MAP_READ (0x0004)
#define SECTION_MAP_EXECUTE (0x0008)
#define SECTION_EXTEND_SIZE (0x0010)
#define SECTION_ALL_ACCESS                                                     \
  (STANDARD_RIGHTS_REQUIRED | SECTION_QUERY | SECTION_MAP_WRITE |              \
   SECTION_MAP_READ | SECTION_MAP_EXECUTE | SECTION_EXTEND_SIZE)

#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS
#define GENERIC_READ (0x80000000)
#define GENERIC_WRITE (0x40000000)
#define OPEN_EXISTING (3)

typedef struct _IMAGE_DOS_HEADER {
  USHORT e_magic;
  USHORT e_cblp;
  USHORT e_cp;
  USHORT e_crlc;
  USHORT e_cparhdr;
  USHORT e_minalloc;
  USHORT e_maxalloc;
  USHORT e_ss;
  USHORT e_sp;
  USHORT e_csum;
  USHORT e_ip;
  USHORT e_cs;
  USHORT e_lfarlc;
  USHORT e_ovno;
  USHORT e_res[4];
  USHORT e_oemid;
  USHORT e_oeminfo;
  USHORT e_res2[10];
  LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_FILE_HEADER {
  USHORT Machine;
  USHORT NumberOfSections;
  ULONG TimeDateStamp;
  ULONG PointerToSymbolTable;
  ULONG NumberOfSymbols;
  USHORT SizeOfOptionalHeader;
  USHORT Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  ULONG VirtualAddress;
  ULONG Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
  USHORT Magic;
  UCHAR MajorLinkerVersion;
  UCHAR MinorLinkerVersion;
  ULONG SizeOfCode;
  ULONG SizeOfInitializedData;
  ULONG SizeOfUninitializedData;
  ULONG AddressOfEntryPoint;
  ULONG BaseOfCode;
  ULONG BaseOfData;
  ULONG ImageBase;
  ULONG SectionAlignment;
  ULONG FileAlignment;
  USHORT MajorOperatingSystemVersion;
  USHORT MinorOperatingSystemVersion;
  USHORT MajorImageVersion;
  USHORT MinorImageVersion;
  USHORT MajorSubsystemVersion;
  USHORT MinorSubsystemVersion;
  ULONG Win32VersionValue;
  ULONG SizeOfImage;
  ULONG SizeOfHeaders;
  ULONG CheckSum;
  USHORT Subsystem;
  USHORT DllCharacteristics;
  ULONG SizeOfStackReserve;
  ULONG SizeOfStackCommit;
  ULONG SizeOfHeapReserve;
  ULONG SizeOfHeapCommit;
  ULONG LoaderFlags;
  ULONG NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef IMAGE_OPTIONAL_HEADER32 IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32 PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS {
  ULONG Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32;

typedef IMAGE_NT_HEADERS32 IMAGE_NT_HEADERS;

typedef struct _IMAGE_SECTION_HEADER {
  UCHAR Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    ULONG PhysicalAddress;
    ULONG VirtualSize;
  } Misc;
  ULONG VirtualAddress;
  ULONG SizeOfRawData;
  ULONG PointerToRawData;
  ULONG PointerToRelocations;
  ULONG PointerToLinenumbers;
  USHORT NumberOfRelocations;
  USHORT NumberOfLinenumbers;
  ULONG Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_EXPORT_DIRECTORY {
  ULONG Characteristics;
  ULONG TimeDateStamp;
  USHORT MajorVersion;
  USHORT MinorVersion;
  ULONG Name;
  ULONG Base;
  ULONG NumberOfFunctions;
  ULONG NumberOfNames;
  ULONG AddressOfFunctions;
  ULONG AddressOfNames;
  ULONG AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _FILETIME {
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME;

typedef struct _WIN32_FIND_DATAA {
  DWORD dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD nFileSizeHigh;
  DWORD nFileSizeLow;
  DWORD dwReserved0;
  DWORD dwReserved1;
  CHAR cFileName[MAX_PATH];
  CHAR cAlternateFileName[14];
#ifdef _MAC
  DWORD dwFileType;
  DWORD dwCreatorType;
  WORD wFinderFlags;
#endif
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef WIN32_FIND_DATAA WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

typedef struct _SECURITY_ATTRIBUTES {
  DWORD nLength;
  LPVOID lpSecurityDescriptor;
  BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#endif // WINDOWS_WRAPPER_H
