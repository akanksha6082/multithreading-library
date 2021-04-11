#ifndef __ATHREAD_TYPES_H
#define __ATHREAD_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/types.h>
#include <stdint.h>
#include <sched.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/futex.h>
#include <syscall.h>
#include <asm/prctl.h>
#include <assert.h>
#include <sys/prctl.h>
#include <stdatomic.h>
#include <signal.h>
#include <ucontext.h>


typedef pid_t athread_t;
typedef void *vptr_t;
typedef uint64_t _uint;

/*thread start routine*/
typedef void * (*thread_start_t)(void *);


/*thread state*/
typedef enum athread_state {
    
    RUNNABLE,
    RUNNING,
    WAITING,
    EXITED
    
}athread_state_t;

/*detach state*/
typedef enum detach_state{
    
    ATHREAD_CREATE_JOINABLE,
    ATHREAD_CREATE_DETACHED,
    ATHREAD_CREATE_JOINED,
    
}detachstate;

/*thread control block*/
typedef struct athread {
    
    /*thread id*/
    athread_t tid;


    /*thread start routine*/
    thread_start_t start_routine;


    /*function arguments*/
    vptr_t args;


    /*return value of the thread start routine*/
    vptr_t return_value;


    /*thread state*/
    athread_state_t thread_state;

    /*thread id of waiting thread*/
    athread_t joining_on;


    /*detachstate*/
    int detachstate;

    /*thread context*/
    jmp_buf *thread_context;


} athread;


#endif