#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <stdint.h>

typedef struct {
    uint16_t len;
    uint16_t maxLen;
    uint16_t *buffer;
} UnicodeStr;

typedef struct _ListEntry {
    struct _ListEntry *flink;
    struct _ListEntry *blink;
} ListEntry;

typedef struct {
    ListEntry moduleList;
    void *res1[2];
    void *dllBase;
    void *entryPoint;
    void *res3;
    UnicodeStr dllName;
} LdrEntry;

typedef struct {
    uint8_t res1[8];
    void *res2[3];
    ListEntry moduleList;
} PEBLdrData;

typedef struct {
    uint8_t res1[2];
    uint8_t isDebugged;
    uint8_t res2[1];
    void *res3;
    void *imageBase;
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
void *get_image_base(void);
void *get_kernel32_base(void);

#endif // PROCESS_INFO_H
