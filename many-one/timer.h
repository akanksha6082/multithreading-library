#ifndef __TIMER_H
#define __TIMER_H

#define TIMER_INTERRUPT (suseconds_t)10000

#include<sys/time.h>

typedef struct itimerval athread_timer_t;
static athread_timer_t timer;

void timer_enable(athread_timer_t  *timer);
void timer_disable(athread_timer_t *timer);

void block_signal(void);
void block_all_signals(void);

void unblock_signal(void);
void unblock_all_signals(void);

#endif