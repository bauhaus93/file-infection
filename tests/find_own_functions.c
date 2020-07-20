#include <stdbool.h>
#include <stdio.h>

#include "checksum.h"
#include "code_begin.h"
#include "code_end.h"
#include "delta.h"
#include "disasm/analysis/analysis.h"
#include "disasm/analysis/block.h"
#include "disasm/analysis/call.h"
#include "disasm/disasm.h"
#include "function_kernel32.h"
#include "infect.h"
#include "infection_thread.h"
#include "memory.h"
#include "oep.h"
#include "pe.h"
#include "process_info.h"
#include "string_generator.h"
#include "utility.h"

int main(int argc, char **argv) {
	const char * function_names[] = { "get_string", NULL };
	const void * function_addresses[] = { (void*)get_string, NULL };

    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread};

    CodeAnalysis *analysis =
        analyze_code(entrypoints, 2, (void *)code_begin, (void *)code_end);

	if (analysis == NULL) {
		fprintf(stderr, "analyze_code() returned NULL");
		return 1;
	}

	for (BlockList * bl = analysis->block_list; bl != NULL; bl = bl->next) {
		if (bl->block == NULL) {
			fprintf(stderr, "BlockList has block with NULL");
			return 1;
		}
		void * addr = bl->block->start;
		for (int i = 0; function_addresses[i] != NULL && function_names[i] != NULL; i++) {
			if (addr == function_addresses[i]) {
				printf("Function: 0x%p -> %20s\n", addr, function_names[i]);
			}
		}
	}

    return 0;
}
