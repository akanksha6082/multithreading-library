#ifndef __TIMER_H
#define __TIMER_H

#include "sys/time.h"
#define TIMER_INTERRUPT (suseconds_t)10000

typedef struct itimerval athread_timer_t;
static athread_timer_t timer;

void interrupt_enable(athread_timer_t  *timer);
void interrupt_disable(athread_timer_t *timer);

#endif