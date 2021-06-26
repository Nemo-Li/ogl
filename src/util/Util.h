//
// Created by Nemo li on 2021/6/26.
//
#include <event.h>

#ifndef OGL_UTIL_H
#define OGL_UTIL_H

static long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_usec;
}

#endif //OGL_UTIL_H
