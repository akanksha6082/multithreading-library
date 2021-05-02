#define _GNU_SOURCE         
#include <unistd.h>
#include <sys/syscall.h>
#include <stdatomic.h>
#include <assert.h>
#include <errno.h>
#include <linux/futex.h>

#include "../include/athread.h"

/*    
 *  futex syscall
 *  param[1] - Pointer to the futex word,
 *  param[2] - futex_op operation to be performed,
 *  param[3] - expected value of the futex word
 *  return value -  0 else errno
 */

static inline int _futex(int *addr, int futex_op, int val) {

    /* use the system call wrapper around the futex system call */
    return syscall(SYS_futex, addr, futex_op, val, NULL, NULL, 0);
}


/*system call wrapper to atomic(non-interruptible) hardware instruction*/
static inline int compare_and_swap(int * object, int old_value, int new_value ){
    
    assert(object);
    return atomic_compare_exchange_strong(object, &old_value, new_value);
    
}


/*spinlock functions*/


/*spin lock intialization*/
int athread_spin_init(athread_spinlock_t * spinlock){
    
    /*check for errors*/
    if(!spinlock)
        return EINVAL;
    
    /* set the owner to none */
    spinlock->owner_thread = -1;

    /* set the lock word status to not taken */
    spinlock->lock = SPINLOCK_NOT_ACQUIRED;

    /*set the lock state*/
    spinlock->state = ACTIVE; 

    return 0;
}


/*lock acquisition*/
int athread_spin_lock(athread_spinlock_t *spinlock){
    
    /*check for errors*/
    if(!spinlock)
        return EINVAL;
    
    /*check for the errors*/
    if(spinlock->state == DESTROYED){
        return -1;
    }

    /*while we don't get the lock*/
    while (!compare_and_swap(&spinlock->lock, SPINLOCK_NOT_ACQUIRED, SPINLOCK_ACQUIRED));

    /* set the owner of the lock */
    spinlock->owner_thread = athread_self();
    
    return 0;
}

/*spin lock release*/
int athread_spin_unlock(athread_spinlock_t *spinlock){

    if(!spinlock)
        return EINVAL;
    
    /*check for errors*/
    if(spinlock->state == DESTROYED){
        return -1;
    }

    /*if the current thread does not own the lock*/
    if(spinlock->owner_thread != athread_self()){
        return EACCES;
    }

    /* release the spinlock */
    if (!compare_and_swap(&spinlock->lock, SPINLOCK_ACQUIRED, SPINLOCK_NOT_ACQUIRED)) {

        /* set the owner thread to none */
        spinlock->lock = SPINLOCK_NOT_ACQUIRED;
        spinlock->owner_thread = -1;
        
    }
    return 0;
}

/*spinlock - destroy*/
int athread_spin_destroy(athread_spinlock_t *spinlock){
    
    /*check for errors*/
    if(!spinlock){
        return EINVAL;
    }

    /*check for errors*/
    if(spinlock->lock == SPINLOCK_ACQUIRED){
        return EBUSY;
    }

    spinlock->state = DESTROYED;
    spinlock->lock = SPINLOCK_NOT_ACQUIRED;
    spinlock->owner_thread = -1;
    
    return 0;

}


/*Mutex functions*/


/*mutex initialization*/
int athread_mutex_init(athread_mutex_t * mutex){
    
    /*set the lock owner to none*/
    mutex->owner = -1;

    /*set the locaked value to not acquired*/
    mutex->locked_value = 0;

    /*lock state*/
    mutex->state = ACTIVE;

    return 0;

}

/*lock acquisition*/
int athread_mutex_lock(athread_mutex_t * mutex){

    if(mutex == NULL)
        return EINVAL;

    /*check for the errors*/
    if(mutex->state == DESTROYED){
        return -1;
    }
 
    /*check if lock is acquired using atomic instruction*/
    while( !compare_and_swap(&mutex->locked_value, 0, 1)){

        _futex(&mutex->locked_value, FUTEX_WAIT, 1);
        
    }
    
    /*set the lock owner*/
    mutex->owner = athread_self();
   
    return 0;

}

/*lock release*/
int athread_mutex_unlock(athread_mutex_t * mutex){

    /*check for errors*/
    if(mutex == NULL)
        return EINVAL;

    /*check for errors*/
    if(mutex->state == DESTROYED){
        return -1;
    }
    
    /*check for errors*/
    if(mutex->owner != athread_self()){
        return EACCES;
    }
    
    /*change the locked value*/
    if(!compare_and_swap(&mutex->locked_value, 1, 0)){
        mutex->owner = -1;
    }
    
    /*invoke futex wake operation*/
    _futex(&mutex->locked_value, FUTEX_WAKE, 1);
    
    return 0;

}

/*mutex - destroy*/
int athread_mutex_destroy(athread_mutex_t * mutex){
    
    /*check for error*/
    if(!mutex){
        return EINVAL;
    }

    /*mutex in locked state*/
    if(mutex->locked_value == 1){
        return EBUSY;
    }
 
    /*change the state of mutex*/
    mutex->state = DESTROYED;
    mutex->locked_value = 0;
    mutex->owner = -1;
    
    return 0;
}


