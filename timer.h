
#ifndef _TIMER_H_

typedef struct {
    struct timespec start;
    struct timespec end;
} Timer;

void timer_start(Timer* timer);
void timer_stop(Timer* timer);
unsigned long long timer_duration_ns(Timer* timer);

#endif
#define _TIMER_H_