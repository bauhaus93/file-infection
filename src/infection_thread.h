#ifndef INFECTION_THREAD_H
#define INFECTION_THREAD_H

#include "windows_wrapper.h"

void spawn_infection_thread(void);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
DWORD WINAPI infection_thread(LPVOID param);
#pragma GCC diagnostic pop

#endif
