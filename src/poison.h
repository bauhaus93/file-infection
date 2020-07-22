#ifndef POISON_H
#define POISON_H

#ifdef ENABLE_POISON
#pragma GCC poison printf fprintf memset memcpy
#endif

#endif // POISON_H
