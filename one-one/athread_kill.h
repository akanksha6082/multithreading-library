#ifndef _THREAD_KILL_H_
#define _THREAD_KILL_H_

#include "athread_types.h"

/*
    Thread kill function signature
*/
ThreadReturn athread_kill(thread a_thread, int sig_num);

#endif