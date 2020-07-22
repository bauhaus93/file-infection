#include <stdio.h>
#include <assert.h>

#include "code_begin.h"
#include "code_end.h"
#include "disasm/analysis/analysis.h"

int main(int argc, char **argv) {
    void *entrypoints[] = {(void *)spawn_infection_thread,
                           (void *)infection_thread};

	CodeAnalysis * analysis = analyze_code(entrypoints, 2, (void*)code_begin, (void*)code_end);
	assert(analysis != NULL);
	size_t code_size = get_code_size(analysis);
	assert(code_size > 0);

}
