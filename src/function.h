#ifndef FUNCTION_H
#define FUNCTION_H

#include "function_list.h"
#include "data.h"

#define EXIT_PROCESS(a) (get_data()->function_list.exit_process(a))
#define GET_TICK_COUNT (get_data()->function_list.get_tick_count())
#define VIRTUAL_ALLOC(a, b, c, d) (get_data()->function_list.virtual_alloc(a, b, c, d))
#define VIRTUAL_FREE(a, b, c) (get_data()->function_list.virtual_free(a, b, c))
#define VIRTUAL_PROTECT(a, b, c, d) (get_data()->function_list.virtual_protect(a, b, c, d))
#define FIND_FIRST_FILE_A(a, b) (get_data()->function_list.find_first_file_a(a, b))
#define FIND_NEXT_FILE_A(a, b) (get_data()->function_list.find_next_file_a(a, b))
#define FIND_CLOSE(a) (get_data()->function_list.find_close(a))

#endif // FUNCTION_H
