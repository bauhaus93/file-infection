#include <stdbool.h>
#include <stdint.h>

#include "analysis.h"
#include "disasm/analysis/function.h"
#include "disasm/disasm.h"
#include "disasm/instruction.h"
#include "malloc.h"
#include "utility.h"

static void *get_next_entrypoint(CallList **pending_calls,
                                 CallList **checked_calls);

static CallList *collect_new_calls(CallList *pending_calls,
                                   CallList *checked_calls, Function *function,
                                   void *min_addr, void *max_addr);

CodeAnalysis *analyze_code(void **entry_points, size_t entrypoint_count,
                           void *min_addr, void *max_addr) {
    CodeAnalysis *analysis = (CodeAnalysis *)MALLOC(sizeof(CodeAnalysis));

    CallList *pending_calls = NULL;
    for (size_t i = 0; i < entrypoint_count; i++) {
        pending_calls = push_call(entry_points[i], pending_calls);
    }
    CallList *checked_calls = NULL;

    int function_count = 0;
    while (pending_calls != NULL) {
        function_count++;
        void *entrypoint = get_next_entrypoint(&pending_calls, &checked_calls);

        analysis->function_list =
            analyze_function(entrypoint, analysis->function_list);
        if (analysis->function_list == NULL) {
            free_call_list(pending_calls);
            free_call_list(checked_calls);
            free_code_analysis(analysis);
            return NULL;
        }
        pending_calls = collect_new_calls(pending_calls, checked_calls,
                                          top_function(analysis->function_list),
                                          min_addr, max_addr);
    }
    PRINT_DEBUG("Found %d functions", function_count);
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

static CallList *collect_new_calls(CallList *pending_calls,
                                   CallList *checked_calls, Function *function,
                                   void *min_addr, void *max_addr) {
    CallList *new_calls = collect_calls_from_function(function);
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
        free_function_list(analysis->function_list);
    }
}

void print_analysis(const CodeAnalysis * analysis) {
	if (analysis != NULL) {
		PRINT_DEBUG("### Code analysis ###");
		for (FunctionList * fl = analysis->function_list; fl != NULL; fl = fl->next) {
			print_function(fl->function);		
		}
	}
}
