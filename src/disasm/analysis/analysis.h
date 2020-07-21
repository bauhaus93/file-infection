#ifndef DISASM_ANALYSIS_H
#define DISASM_ANALYSIS_H

#include "block.h"

typedef struct {
    BlockList *block_list;
    int function_count;
} CodeAnalysis;

CodeAnalysis *analyze_code(void **entry_points, size_t entrypoint_count,
                           void *min_addr, void *max_addr);
void free_code_analysis(CodeAnalysis *analysis);
size_t get_code_size(const CodeAnalysis *analysis);
void print_analysis(const CodeAnalysis *analysis);

#endif // DISASM_ANALYSIS_H
