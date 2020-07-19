#include <stdbool.h>

#include "call.h"
#include "malloc.h"

CallList *push_call(void *call_address, CallList *calls) {
    CallList *new_call = (CallList *)MALLOC(sizeof(CallList));
    new_call->address = call_address;
    new_call->next = calls;
    return new_call;
}

void free_call_list(CallList *calls) {
    while (calls != NULL) {
        CallList *next = calls->next;
        FREE(calls);
        calls = next;
    }
}

void *top_call(CallList *calls) {
    if (calls != NULL) {
        return calls->address;
    }
    return NULL;
}

CallList *pop_call(CallList *calls) {
    if (calls != NULL) {
        CallList *new_root = calls->next;
        FREE(calls);
        return new_root;
    }
    return NULL;
}

bool call_in_list(void *call_address, CallList *calls) {
    for (CallList *ptr = calls; ptr != NULL; ptr = ptr->next) {
        if (ptr->address == call_address) {
            return true;
        }
    }
    return false;
}
