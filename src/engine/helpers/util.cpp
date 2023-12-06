//
// Created by Nemo li on 2021/12/1.
//

#include "util.h"
#include <cstdlib>
#include <sys/time.h>

long long GetCurrentTimeMillis() {
#ifdef WIN32
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, nullptr);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif
}