#ifndef BLOCK_DISCOVERY_H
#define BLOCK_DISCOVERY_H

#include "block.h"

BlockList *discover_blocks(void **entry_points, void *min_addr, void *max_addr);

#endif // BLOCK_DISCOVERY_H
