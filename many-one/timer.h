#ifndef __TIMER_H
#define __TIMER_H

#include "athread.h"

void interrupt_enable(athread_timer_t  *timer);
void interrupt_disable(athread_timer_t *timer);

#endif