#pragma once
#include <time.h>

typedef int Work; // could be any type of data (int[], struct etc)


static inline void sleep_by_work_unit(Work work_unit)
{
    struct timespec sleep_spec = {
        .tv_sec=0,
        .tv_nsec = (4 + (work_unit % 3)) * 1000L * 10
    };
    nanosleep(&sleep_spec, NULL);
}
