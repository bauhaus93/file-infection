#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "disasm/instruction.h"
#include "disasm/analysis/function.h"

typedef struct {
	FunctionList * function_list;
} CodeAnalysis;

CodeAnalysis *analyze_code(void **entry_points, size_t entrypoint_count, void *min_addr, void *max_addr);
void free_code_analysis(CodeAnalysis * analysis);
void print_analysis(const CodeAnalysis * analysis);

#endif // ANALYSIS_H
