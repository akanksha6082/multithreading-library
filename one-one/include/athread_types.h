#ifndef __ATHREAD_TYPES_H
#define __ATHREAD_TYPES_H

#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>

/* spinlock status info */
#define SPINLOCK_ACQUIRED       (1u)
#define SPINLOCK_NOT_ACQUIRED   (0u)


typedef pid_t athread_t;

typedef void* vptr_t;

typedef uint64_t _uint;

/*start thread routine*/
typedef void * (*thread_start_t)(void *);


/*thread state enumeration*/
enum {

    ATHREAD_CREATE_DETACHED,
    ATHREAD_CREATE_JOINABLE,
    ATHREAD_CREATE_JOINED,
    ATHREAD_CREATE_EXITED
};


/*return status of the thread library*/
typedef enum _ThreadReturn {

    THREAD_SUCCESS,						
    THREAD_FAIL                		

} ThreadReturn;


/*thread control block */
typedef struct athread{
    
    /*thread ID*/
    athread_t tid;

    /*thread state*/
    int thread_state;


    /*stack base pointer*/
    vptr_t stack_base;


    /*stack size*/
    size_t stack_size;


    /*thread start Routine*/
    thread_start_t start_routine;


    /*thread arguments*/
    vptr_t args;

    
    /*thread return value */
    vptr_t return_value;

    
    /*futex word*/
    int32_t futex;	

    
    /*thread context*/
    jmp_buf thread_context;

    /*wait*/
    athread_t joining_on;


}athread;



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


/*thread spinlocks*/
typedef struct athread_spinlock {
    
    /*owner of the lock*/
    athread_t owner_thread;

    /*lock word*/
    int lock;

    /*lock status*/
    lock_state_t state;
    
}athread_spinlock_t;



/*mutex structure*/
typedef struct athread_mutex{
    
    /*lock value*/
    int locked_value;

    /*thread id of thread currently owning lock*/
    athread_t owner;

    /*state of mutex lock*/
    lock_state_t state;

}athread_mutex_t;

#endif