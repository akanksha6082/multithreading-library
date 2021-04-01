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
#include <sys/prctl.h>




typedef pid_t athread_t;

typedef void *ptr_t;

typedef uint64_t _uint;




/*
  Start Thread Routine
*/
typedef void * (*thread_start_t)(void *);


/*
    Thread state enumeration
*/
enum {

    ATHREAD_CREATE_DETACHED,
    ATHREAD_CREATE_JOINABLE,
    ATHREAD_CREATE_JOINED,
    ATHREAD_CREATE_EXITED
};


/*
   Return statuses of the thread library
*/
typedef enum _ThreadReturn {

    THREAD_SUCCESS,						
    THREAD_FAIL                		

} ThreadReturn;


/*
  Define Thread control block
*/
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

/*
    Thread Handle for User Handling
*/
typedef athread *thread;

/*
    Thread Spinlocks defination
*/
typedef struct _athread_SpinLock {
    
    /*Owner of the lock*/
    athread owner_thread;

    /*The Lock Word on which we will perform locking*/
    int lock;
}athread_SpinLock;

/*
    Spinlock Status Info
*/
#define SPINLOCK_ACQUIRED       (0u)
#define SPINLOCK_NOT_ACQUIRED   (1u)


typedef struct athread_attr_t {
    
    /* detach state of thread */
    int detach_state;

    /*pointer to stack base*/
    ptr_t stack_addr;

    /*stack size*/
    size_t stack_size;
    

} athread_attr_t;


#define TCB_SIZE (sizeof(athread))

#endif