#include "athread.h"

static inline int _futex(int *addr, int futex_op, int val) {

    /* use the system call wrapper around the futex system call */
    return syscall(SYS_futex, addr, futex_op, val, NULL, NULL, 0);
}

static inline int compare_and_swap(int * object, int old_value, int new_value ){
    
    assert(object);
    return atomic_compare_exchange_strong(object, &old_value, new_value);
    
}


int athread_mutex_init(athread_mutex_t * mutex){
    
    mutex->owner = -1;
    mutex->locked_value = 0;
    mutex->state = ACTIVE;
    return 0;

}

int athread_mutex_lock(athread_mutex_t * mutex){

    if(mutex == NULL)
        return EINVAL;

    /**check for the errors**/
    if(mutex->state == DESTROYED){
        return -1;
    }
 
    /*check if lock is acquired using atomic instruction*/
    while( !compare_and_swap(&mutex->locked_value, 0, 1)){

        _futex(&mutex->locked_value, FUTEX_WAIT, 1);
        
    }
    
    mutex->owner = athread_self();
   
    return 0;

}


int athread_mutex_unlock(athread_mutex_t * mutex){

    if(mutex == NULL)
        return EINVAL;

    /*check for errors*/
    if(mutex->state == DESTROYED){
        return -1;
    }
    
    if(mutex->owner != athread_self()){
        return EACCES;
    }
    
    /*change the locked value*/
    if(!compare_and_swap(&mutex->locked_value, 1, 0)){
        mutex->owner = -1;
    }
    

    /*invoke futex wake operation*/
    int ret_val = _futex(&mutex->locked_value, FUTEX_WAKE, 1);
    
    return 0;

}


int athread_mutex_destroy(athread_mutex_t * mutex){
    
    /*check for error*/
    if(!mutex){
        return EINVAL;
    }

    /*mutex in locked state*/
    if(mutex->locked_value == 1){
        return EBUSY;
    }
 
    /*mark the mutex as destroyed*/
    mutex->state = DESTROYED;
    
    /*free the memory allocated to mutex*/
    free(mutex);

    return 0;
}


