#include "athread.h"

static inline int _futex(int *addr, int futex_op, int val) {

    /* Use the system call wrapper around the futex system call */
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

/*masks the signals for calling thread*/
static int disable_interrupt(){
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    if(sigprocmask(SIG_BLOCK, &set, NULL) < 0){
        return errno;
    }

    return 0;
}


/*unmasks the signals for calling thread*/
static int enable_interrupt(){

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);

    if(sigprocmask(SIG_UNBLOCK, &set, NULL) < 0){
        return errno;
    }

    return 0;

}


int athread_mutex_lock(athread_mutex_t * mutex){

    if(mutex == NULL)
        return EINVAL;

    //check for the errors
    if(mutex->state == DESTROYED){
        return -1;
    }

    //disable the timer interrupt
    disable_interrupt();

    //check if lock is acquired or not using atomic instruction
    int old_value = compare_and_swap(&mutex->locked_value, 0, 1);
    
    if(!old_value){

        //lock can be acquired
        mutex->state = 1;
        athread *target_thread = athread_self();
        mutex->owner = target_thread->tid;
    }
    else{
        //cause the calling thread to sleep until the lock is available
        _uint ret_val = _futex(&mutex->locked_value, FUTEX_WAIT, 1);
        
        if(errno != EAGAIN && ret_val == -1){
            return EINVAL;
        }
    }
   
    return 0;

}


int athead_mutex_unlock(athread_mutex_t * mutex){

    if(mutex == NULL)
        return EINVAL;

    /*check for errors*/
    if(mutex->state == DESTROYED || mutex->owner != athread_self()){
        return -1;
    }
    
    /*change the locked value*/
    mutex->locked_value = 0;

    /*invoke futex wake operation*/
    int ret_val = _futex(&mutex->locked_value, FUTEX_WAKE, 1);
    
    if(ret_val == -1 && ret_val != EINVAL){
        
        mutex->owner = -1;
        return EINVAL;
    }

    /*enable the timer interrupt*/ 
    enable_interrupt();

    return 0;

}

int athread_mutex_destroy(athread_mutex_t * mutex){
    
    /*check for error*/
    if(!mutex){
        return EINVAL;
    }

    if(mutex->locked_value == 1){
        return EBUSY;
    }

    if(mutex->owner != athread_self())
        return EINVAL;
    
    mutex->state = DESTROYED;
    
    free(mutex);

    return 0;
}


