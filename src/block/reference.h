#ifndef DISASM_ANALYSIS_REFERENCE_H
#define DISASM_ANALYSIS_REFERENCE_H

#include <stdbool.h>

#include "../disasm/instruction.h"

typedef struct ReferenceList_ {
  void *dest;
  void *src;
  struct ReferenceList_ *next;
} ReferenceList;

ReferenceList *push_reference(void *dest, void *src, ReferenceList *references);
ReferenceList *pop_reference(ReferenceList *references);
void *top_reference_dest(ReferenceList *references);
void free_reference_list(ReferenceList *references);
bool reference_in_list(void *dest, ReferenceList *references);

#endif // DISASM_ANALYSIS_REFERENCE_H
