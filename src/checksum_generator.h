#ifndef CHECKSUM_GENERATOR_H
#define CHECKSUM_GENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "checksum.h"

#define BUFFER_SIZE (256)

struct HashSet {
    const char* string;
    uint32_t hash;
    struct HashSet* next;
};

typedef struct HashSet hashset_t;

int generate_checksums(const char* fileIn, const char* fileOut, const char* safeguard);
void free_list(hashset_t* root);
int check_uniqueness(hashset_t* newEntry, hashset_t* root);
char* create_safeguard_string(const char* filename);

#endif
