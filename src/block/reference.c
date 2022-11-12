#include <stdbool.h>

#include "malloc.h"
#include "reference.h"

ReferenceList *push_reference(void *dest, void *src,
                              ReferenceList *references) {
  ReferenceList *new_reference = (ReferenceList *)MALLOC(sizeof(ReferenceList));
  new_reference->dest = dest;
  new_reference->src = src;
  new_reference->next = references;
  return new_reference;
}

void free_reference_list(ReferenceList *references) {
  while (references != NULL) {
    ReferenceList *next = references->next;
    FREE(references);
    references = next;
  }
}

void *top_reference_dest(ReferenceList *references) {
  if (references != NULL) {
    return references->dest;
  }
  return NULL;
}

ReferenceList *pop_reference(ReferenceList *references) {
  if (references != NULL) {
    ReferenceList *new_root = references->next;
    FREE(references);
    return new_root;
  }
  return NULL;
}

bool reference_in_list(void *dest, ReferenceList *references) {
  for (ReferenceList *ptr = references; ptr != NULL; ptr = ptr->next) {
    if (ptr->dest == dest) {
      return true;
    }
  }
  return false;
}
