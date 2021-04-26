#ifndef __ATHREAD_TYPES_H
#define __ATHREAD_TYPES_H

#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>


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
    
    ATHREAD_CREATE_DETACHED,
    ATHREAD_CREATE_JOINABLE,
    ATHREAD_CREATE_JOINED,
    ATHREAD_CREATE_EXITED,
    
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

    
    /*stack base*/
    vptr_t stack_base;


    /*thread state*/
    athread_state_t thread_state;

    
    /*thread id of waiting thread*/
    athread_t joining_on;


    /*detachstate*/
    int detachstate;

    /*thread context*/
    sigjmp_buf thread_context;

    /*set of pending signals*/
    sigset_t pending_signals;

} athread;


/*thread attribute structure*/
typedef struct athread_attr_t {
    
    /* detach state of thread */
    int detach_state;


    /*pointer to stack base*/
    vptr_t stack_addr;


    /*stack size*/
    size_t stack_size;
    

} athread_attr_t;

/*lock status enumeration*/
typedef enum lock_state {
    ACTIVE,
    DESTROYED,
} lock_state_t;


typedef struct athread_spinlock{
    
    /*owner of the lock*/
    athread_t owner_thread;

    /*lock word*/
    int lock;

    /*lock status*/
    lock_state_t state;

}athread_spinlock_t;


#endif