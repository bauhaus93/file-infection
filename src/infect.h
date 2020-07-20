#ifndef INFECT_H
#define INFECT_H

#define INFECTION_MARKER (0xDEADBEEF)

int infect(const char *filename, void *entry_function_addr);

#endif
