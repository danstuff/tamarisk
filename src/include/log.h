#ifndef UTILITY_H
#define UTILITY_H

#include "global.h"

#if BUILD_LEVEL <= DEBUG
#define massert(condition)\
if (!(condition))\
{\
    merrf("%s:%i Assertion %s failed.", __FILE__, __LINE__, #condition);\
}
#else
#define massert(condition)
#endif

void mlogf(const char* msg, ...);
void mwarnf(const char* msg, ...);
void merrf(const char* msg, ...);

u_int64_t mtime_us();
u_int64_t mtime_s();

#endif
