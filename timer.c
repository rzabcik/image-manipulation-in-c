
// Inspired by Dr. Sreepathi Pai's Timer.h

#include <time.h>
#include <unistd.h>
#include "timer.h"

#ifdef _POSIX_MONOTONIC_CLOCK
#ifdef CLOCK_MONOTONIC_RAW
static clockid_t CLOCKTYPE = CLOCK_MONOTONIC_RAW;
#else
static clockid_t CLOCKTYPE = CLOCK_MONOTONIC;
#endif /* CLOCK_MONOTONIC_RAW */
#else
#warning "CLOCK_MONOTONIC is unavailable, using CLOCK_REALTIME"
static clockid CLOCKTYPE = CLOCK_REALTIME;
#endif /* _POSIX_MONOTONIC_CLOCK */

#define NANOSEC 1000000000LL

void timer_start(Timer* timer) {
    clock_gettime(CLOCKTYPE, &timer->start);
    timer->end = timer->start;
}

void timer_stop(Timer* timer) {
    clock_gettime(CLOCKTYPE, &timer->end);
}

unsigned long long timer_duration_ns(Timer* timer) {
    return (timer->end.tv_sec * NANOSEC + timer->end.tv_nsec) - (timer->start.tv_sec * NANOSEC + timer->start.tv_nsec);
}
