#ifndef POISON_H
#define POISON_H

#ifdef ENABLE_POISON
#pragma GCC poison printf, fprintf
#endif

#endif // POISON_H
