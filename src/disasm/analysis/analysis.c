#include <stdbool.h>
#include <stdint.h>

#include "analysis.h"
#include "block.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "malloc.h"
#include "utility.h"

static void *get_next_entrypoint(CallList **pending_calls,
                                 CallList **checked_calls);

static CallList *collect_new_calls(CallList *pending_calls,
                                   CallList *checked_calls,
                                   BlockList *block_list, void *min_addr,
                                   void *max_addr);

CodeAnalysis *analyze_code(void **entry_points, size_t entrypoint_count,
                           void *min_addr, void *max_addr) {
    CodeAnalysis *analysis = (CodeAnalysis *)MALLOC(sizeof(CodeAnalysis));
    analysis->function_count = 0;
    analysis->block_list = NULL;

    CallList *pending_calls = NULL;
    for (size_t i = 0; i < entrypoint_count; i++) {
        pending_calls = push_call(entry_points[i], pending_calls);
    }
    CallList *checked_calls = NULL;

    while (pending_calls != NULL) {
        analysis->function_count++;
        void *entrypoint = get_next_entrypoint(&pending_calls, &checked_calls);

        analysis->block_list = analyze_block(entrypoint, analysis->block_list);
        if (analysis->block_list == NULL) {
            free_code_analysis(analysis);
            analysis = NULL;
            break;
        }
        if (pending_calls == NULL) {
            pending_calls =
                collect_new_calls(pending_calls, checked_calls,
                                  analysis->block_list, min_addr, max_addr);
        }
    }
    free_call_list(pending_calls);
    free_call_list(checked_calls);
    return analysis;
}

static void *get_next_entrypoint(CallList **pending_calls,
                                 CallList **checked_calls) {
    if (*pending_calls != NULL) {
        void *entrypoint = top_call(*pending_calls);
        *pending_calls = pop_call(*pending_calls);
        *checked_calls = push_call(entrypoint, *checked_calls);
        return entrypoint;
    }
    return NULL;
}

size_t get_code_size(const CodeAnalysis *analysis) {
    size_t sum = 0;
    if (analysis != NULL) {
        for (BlockList *ble = analysis->block_list; ble != NULL;
             ble = ble->next) {
            sum += get_block_size(ble->block);
        }
    }
    return sum;
}

static CallList *collect_new_calls(CallList *pending_calls,
                                   CallList *checked_calls,
                                   BlockList *block_list, void *min_addr,
                                   void *max_addr) {
    CallList *new_calls = collect_calls_from_blocks(block_list);
    while (new_calls != NULL) {
        void *new_target = top_call(new_calls);
        if (new_target >= min_addr && new_target <= max_addr) {
            if (!call_in_list(new_target, pending_calls) &&
                !call_in_list(new_target, checked_calls)) {
                pending_calls = push_call(new_target, pending_calls);
            }
        }
        new_calls = pop_call(new_calls);
    }
    return pending_calls;
}

void free_code_analysis(CodeAnalysis *analysis) {
    if (analysis != NULL) {
        free_block_list(analysis->block_list);
    }
}
