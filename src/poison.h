#ifndef POISON_H
#define POISON_H

#ifndef USE_STANDARD_MALLOC
#pragma GCC poison printf fprintf memset memcpy
#endif

#endif // POISON_H
