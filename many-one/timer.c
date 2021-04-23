#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>

#include "timer.h"

void timer_enable(athread_timer_t *timer){

    timer->it_interval.tv_usec = TIMER_INTERRUPT;
    timer->it_interval.tv_sec = 0;
    timer->it_value.tv_usec = TIMER_INTERRUPT;
    timer->it_value.tv_sec = 0;

    setitimer(ITIMER_VIRTUAL, timer, 0);

    return;
}

void block_signal(void){
    
    sigset_t tmp;
    sigaddset(&tmp, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &tmp, NULL);
    return;

}

void unblock_signal(void){

    sigset_t tmp;
    sigaddset(&tmp, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &tmp, NULL);
    return;
    
}

void block_all_signals(){
    sigset_t tmp;
    sigfillset(&tmp);
    sigprocmask(SIG_BLOCK, &tmp, NULL);
    return;
}

void unblock_all_signals(){
    sigset_t tmp;
    sigfillset(&tmp);
    sigprocmask(SIG_UNBLOCK, &tmp, NULL);
    return;
}
