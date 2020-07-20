#include "disasm/analysis/function.h"
#include "block.h"
#include "malloc.h"
#include "utility.h"

static Function *create_function(void *entrypoint);
static FunctionList *push_function(void *entrypoint,
                                   FunctionList *function_list);
static void free_function(Function *function);

FunctionList *analyze_function(void *entrypoint, FunctionList *function_list) {
    // PRINT_DEBUG("Analyzing function, entrypoint = 0x%p", entrypoint);

    function_list = push_function(entrypoint, function_list);
    if (function_list != NULL) {
        top_function(function_list)->blocks = analyze_block(entrypoint, NULL);
    }

    return function_list;
}

static Function *create_function(void *entrypoint) {
    Function *func = (Function *)MALLOC(sizeof(Function));
    if (func != NULL) {
        func->entrypoint = entrypoint;
        func->blocks = NULL;
    }
    return func;
}

static void free_function(Function *function) {
    if (function != NULL) {
        free_block_list(function->blocks);
        FREE(function);
    }
}

static FunctionList *push_function(void *entrypoint,
                                   FunctionList *function_list) {
    FunctionList *new_list = (FunctionList *)MALLOC(sizeof(FunctionList));
    if (new_list != NULL) {
        new_list->function = create_function(entrypoint);
        if (new_list->function == NULL) {
            free_function_list(function_list);
            free_function_list(new_list);
            return NULL;
        } else {
            new_list->next = function_list;
        }
    } else {
        free_function_list(function_list);
    }
    return new_list;
}

Function *top_function(FunctionList *function_list) {
    if (function_list != NULL) {
        return function_list->function;
    }
    return NULL;
}

void free_function_list(FunctionList *function_list) {
    while (function_list != NULL) {
        FunctionList *next = function_list->next;
        free_function(function_list->function);
        FREE(function_list);
        function_list = next;
    }
}

CallList *collect_calls_from_function(Function *function) {
    if (function != NULL) {
        return collect_calls_from_blocks(function->blocks);
    }
    return NULL;
}

size_t get_function_size(const Function *function) {
    size_t sum = 0;
    if (function != NULL) {
        for (BlockList *bl = function->blocks; bl != NULL; bl = bl->next) {
            sum += bl->block->end - bl->block->start;
        }
    }
    return sum;
}

void print_function(const Function *function) {
    if (function != NULL) {
        PRINT_DEBUG("Function: entrypoint = 0x%p", function->entrypoint);
    }
}
