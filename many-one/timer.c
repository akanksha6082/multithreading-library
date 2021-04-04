#include "timer.h"

void interrupt_enable(athread_timer_t *timer){

    timer->it_interval.tv_usec = TIMER_INTERRUPT;
    timer->it_interval.tv_sec = 0;
    timer->it_value.tv_usec = TIMER_INTERRUPT;
    timer->it_value.tv_sec = 0;

    setitimer(ITIMER_VIRTUAL, timer, 0);

    return;
}

void interrupt_disable(athread_timer_t *timer){

    timer->it_interval.tv_usec = 0;
    timer->it_interval.tv_sec = 0;
    timer->it_value.tv_usec = 0;
    timer->it_value.tv_sec = 0;

    setitimer(ITIMER_VIRTUAL, timer, 0);

    return;
}