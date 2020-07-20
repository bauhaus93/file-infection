#ifndef DISASM_ANALYSIS_CALL_H
#define DISASM_ANALYSIS_CALL_H

#include <stdbool.h>

#include "disasm/instruction.h"

typedef struct CallList_ {
    void *address;
    struct CallList_ *next;
} CallList;

CallList *push_call(void *call_address, CallList *calls);
CallList *pop_call(CallList *calls);
void *top_call(CallList *calls);
void free_call_list(CallList *calls);
bool call_in_list(void *call_address, CallList *calls);

#endif // DISASM_ANALYSIS_CALL_H
