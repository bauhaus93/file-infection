#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct {

} ListEntry;

typedef struct {
    uint8_t     res1[8];
    void*       res2[3];
    ListEntry   moduleList;
} PEBLoaderData;

typedef struct {
    uint8_t         res1[2];
    uint8_t         isDebugged;
    uint8_t         res2[1];
    void*           res3[2];
    PEBLoaderData*  loader;
    //excluding: some bytes of unneeded stuff
} PEB;

typedef struct {
    void*   res1[12];
    PEB*    peb;
    //excluding: some bytes of unneeded stuff
} TEB;

extern void* get_image_base(void);
extern void* get_kernel32_base(void);
extern void  end_code(void);
