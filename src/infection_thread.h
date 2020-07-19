#ifndef INFECTION_THREAD_H
#define INFECTION_THREAD_H

void spawn_infection_thread(void);
DWORD WINAPI infection_thread(LPVOID param);

#endif
