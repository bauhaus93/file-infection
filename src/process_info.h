#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <stdint.h>

typedef struct {
  uint16_t len;
  uint16_t max_len;
  uint16_t *buffer;
} UnicodeStr;

typedef struct _ListEntry {
  struct _ListEntry *flink;
  struct _ListEntry *blink;
} ListEntry;

typedef struct {
  ListEntry module_list;
  void *res1[2];
  void *dll_base;
  void *entry_point;
  void *res3;
  UnicodeStr dll_name;
} LdrEntry;

typedef struct {
  uint8_t res1[8];
  void *res2[3];
  ListEntry module_list;
} PEBLdrData;

typedef struct {
  uint8_t res1[2];
  uint8_t is_debugged;
  uint8_t res2[1];
  void *res3;
  void *image_base;
  PEBLdrData *ldr;
  // excluding: some bytes of unneeded stuff
} PEB;

typedef struct {
  void *res1[12];
  PEB *peb;
  // excluding: some bytes of unneeded stuff
} TEB;

TEB *get_teb(void);
PEB *get_peb(void);
void *get_module_base(uint16_t module_string_id);
void *get_kernel32_base(void);

#endif // PROCESS_INFO_H
