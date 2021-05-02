#define _GNU_SOURCE         
#include <unistd.h>
#include <stdatomic.h>
#include <assert.h>
#include <errno.h>
#include "../include/athread.h"

#define SPINLOCK_NOT_ACCQUIRED 0
#define SPINLOCK_ACCQUIRED 1


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
    spinlock->lock = SPINLOCK_NOT_ACCQUIRED;

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
    while (!compare_and_swap(&spinlock->lock, SPINLOCK_NOT_ACCQUIRED, SPINLOCK_ACCQUIRED));

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
    if (!compare_and_swap(&spinlock->lock, SPINLOCK_ACCQUIRED, SPINLOCK_NOT_ACCQUIRED)) {

        /* set the owner thread to none */
        spinlock->lock = SPINLOCK_NOT_ACCQUIRED;
        spinlock->owner_thread = -1;
        
    }
    return 0;
}

/*spinlock - destroy*/
int athread_spin_destroy(athread_spinlock_t *spinlock){
    
    if(!spinlock){
        return EINVAL;
    }

    if(spinlock->lock == SPINLOCK_ACCQUIRED){
        return EBUSY;
    }

    spinlock->state = DESTROYED;
    spinlock->lock = SPINLOCK_NOT_ACCQUIRED;
    spinlock->owner_thread = -1;
    
    return 0;

}