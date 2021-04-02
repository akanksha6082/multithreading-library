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

/* Spinlock Status Info */
#define SPINLOCK_ACQUIRED       (1u)
#define SPINLOCK_NOT_ACQUIRED   (0u)

typedef pid_t athread_t;

typedef void *ptr_t;

typedef uint64_t _uint;

/*Start Thread Routine*/
typedef void * (*thread_start_t)(void *);


/*Thread state enumeration*/
enum {

    ATHREAD_CREATE_DETACHED,
    ATHREAD_CREATE_JOINABLE,
    ATHREAD_CREATE_JOINED,
    ATHREAD_CREATE_EXITED
};


/*Return statuses of the thread library*/
typedef enum _ThreadReturn {

    THREAD_SUCCESS,						
    THREAD_FAIL                		

} ThreadReturn;


/* Define Thread control block */
typedef struct athread{
    
    /*Thread ID*/
    athread_t tid;

    /*Thread state*/
    int thread_state;


    /*stack base pointer*/
    ptr_t stack_base;


    /*stack size*/
    size_t stack_size;


    /*Thread start Routine*/
    thread_start_t start_routine;


    /*Thread Arguments*/
    ptr_t args;

    
    /*Thread return value */
    ptr_t return_value;

    
    
    /*Futex word*/
    int32_t futex;	

    
    /*Thread context*/
    jmp_buf thread_context;


}athread;

typedef struct athread_attr_t {
    
    /* detach state of thread */
    int detach_state;

    /*pointer to stack base*/
    ptr_t stack_addr;

    /*stack size*/
    size_t stack_size;
    

} athread_attr_t;


#define TCB_SIZE (sizeof(athread))


/*thread spinlocks*/
typedef struct athread_spinlock {
    
    /*owner of the lock*/
    athread_t owner_thread;

    /*the lock word on which we will perform locking*/
    int lock;
    
}athread_spinlock_t;

enum {
    ACTIVE,
    DESTROYED
};

/*mutex structure*/
typedef struct athread_mutex{
    
    /*lock value*/
    int locked_value;

    /*thread id of thread currently owning lock*/
    athread_t owner;

    int state;

}athread_mutex_t;

#endif