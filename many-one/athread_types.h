#ifndef __ATHREAD_TYPES_H
#define __ATHREAD_TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/types.h>
#include <stdint.h>
#include <sched.h>
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
typedef void *ptr_t;
typedef uint64_t _uint;

/*thread start routine*/
typedef void * (*thread_start_t)(void *);


/*thread state*/
enum thread_state {
    
    RUNNABLE,
    RUNNING,
    WAITING,
    EXITED
    
};

/*thread control block*/
typedef struct athread {
    
    /*thread id*/
    athread_t tid;


    /*thread start routine*/
    thread_start_t start_routine;


    /*function arguments*/
    ptr_t args;


    /*return value of the thread start routine*/
    ptr_t return_value;


    /*ptr to base of stack*/
    ptr_t stack_base;


    /*stack size*/
    size_t stack_size;


    /*thread state*/
    int thread_state;


    /*threads context*/
    ucontext_t thread_context;


} athread;


#endif