#ifndef DISASM_ANALYSIS_FUNCTION_H
#define DISASM_ANALYSIS_FUNCTION_H

#include "block.h"
#include "call.h"

typedef struct {
    void *entrypoint;
    BlockList *blocks;
} Function;

typedef struct FunctionList_ {
	Function * function;
	struct FunctionList_ * next;
}FunctionList;

FunctionList *analyze_function(void *entrypoint, FunctionList * function_list);
void free_function_list(FunctionList * function_list);
Function *top_function(FunctionList *function_list);
CallList *collect_calls_from_function(Function *function);
void print_function(const Function * function);

#endif // DISASM_ANALYSIS_FUNCTION_H
