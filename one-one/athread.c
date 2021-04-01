#define _GNU_SOURCE
#include "athread.h"
#include "stackmem.h"


static inline int _get_tls(_uint * addr){
    return syscall(SYS_arch_prctl, ARCH_GET_FS, addr);
}

/*    
 *  Futex syscall Implementation
 *  Param[1] - Pointer to the futex word,
 *  param[2] - futex_op operation to be performed,
 *  param[3] - expected value of the futex word
 *  Return value -  0 / errno
 */

static inline int _futex(int *addr, int futex_op, int val) {

    /* Use the system call wrapper around the futex system call */
    return syscall(SYS_futex, addr, futex_op, val, NULL, NULL, 0);
}



/**
 * Sends a signal to a thread in the thread group
 * param[1]     - tgid Thread group id
 * param[2]     - tid Thread id
 * param[3]     - sig Signal number
 * Return type  - Integer status
 */

static inline int _tgkill(int tgid, int tid, int sig) {

    /* Use the system call wrapper to invoke the call */
    return syscall(SYS_tgkill, tgid, tid, sig);
}



static void _cleanup_thread(athread * thread){

    /* deallocate the stack of thread */
    _deallocate_stack(thread->stack_base, thread->stack_size);

    
    /*change the state of thread*/
    thread->thread_state = ATHREAD_CREATE_JOINED;

    return;
}

static athread * _wrapper_athread_self(void){
    _uint addr;

    int return_value = _get_tls(&addr);

    return (athread*)addr;
    
}

int _wrapper_start(void * argument){
    
    athread * thread_block;
    thread_block = (athread *)argument;


    /*returned to the called location via long jump*/
    if(setjmp(thread_block->thread_context) == 0){
        thread_block->return_value = thread_block->start_routine(thread_block->args);
        
    }
    
    if(thread_block->thread_state == ATHREAD_CREATE_DETACHED){
        free(thread_block);
    }
    
    return 0;
}



/*Initialisation routine*/
int athread_init(){

    qinit(&task_queue);
    max_allowed_threads = get_threads_limit();
    stack_limit = get_stack_limit();
    page_size = get_page_size();

    return 0;
}

/*  
 *  param[1] - Pointer to athread_t variable to store thread_id of new created thread.
 *  param[2] - Pointer to a thread attribute object used to set thread attributes.
 *  param[3] - Pointer to a thread start routine. 
 *  param[4] - arguments to start routine.
 *  Return type - 0 if succesful else error number
 */ 

int athread_create( athread_t *thread, athread_attr_t *attr, thread_start_t start_routine, ptr_t args){
    
    
    /*check for errors */
    if(return_qcount(&task_queue) == max_allowed_threads){
        return EAGAIN;
    }

    if(!thread || !start_routine){
        return EAGAIN;
    }


    /*Allocate thread control block*/ 
    athread * thread_block = (athread*)malloc(TCB_SIZE);

    if (!thread_block) {
        return ENOMEM;
    }

    ptr_t stack_top;

    thread_block->args = args;
    thread_block->start_routine = start_routine;
    thread_block->return_value = 0;
    thread_block->thread_state = attr->detach_state ? attr->detach_state : ATHREAD_CREATE_JOINABLE;
    thread_block->stack_size = attr->stack_size ? attr->stack_size : stack_limit;
    thread_block->stack_base = attr->stack_addr ? attr->stack_addr : NULL ;

    if(thread_block->stack_base == NULL){
        
        /*allocate the stack to thread*/
       
        thread_block->stack_base = _stack_allocate(thread_block->stack_size);
        if(thread_block->stack_base == NULL){
            return ENOMEM;
        }
    }

     /* Get the stack top */
    stack_top = thread_block->stack_base + thread_block->stack_size;


     /* Clone the thread */
    thread_block->tid = clone(_wrapper_start,
                                    stack_top,
                                    CLONE_VM | CLONE_FS | CLONE_FILES |
                                    CLONE_SIGHAND | CLONE_THREAD |
                                    CLONE_SYSVSEM | CLONE_PARENT_SETTID |
                                    CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                                    thread_block,
                                    &thread_block->futex,
                                    thread_block,
                                    &thread_block->futex);

    /*if no kernel thread is allocated*/                                
    if(thread_block->tid == -1){
        
        (thread) = NULL;
        _deallocate_stack(thread_block->stack_base, thread_block->stack_size);
        free(thread_block);

        return EINVAL;
    }

    enqueue(&task_queue, thread_block);
    *thread = thread_block->tid;

    return 0;

}


int athread_join(athread_t thread_id, void ** return_value){
    
    athread * join_thread = search_tcb(&task_queue, thread_id);
    if(join_thread == NULL){
        return ESRCH;
    }

    if(join_thread->thread_state == ATHREAD_CREATE_DETACHED || join_thread->thread_state == ATHREAD_CREATE_JOINED){
        return EINVAL;
    }

    /* Wait on the thread's futex word --returns if the value changes */
    /* kernel checks if the value at addr is the same as val; if so, it then blocks the calling thread or process*/
    _uint ret_val = _futex(&join_thread->futex, FUTEX_WAIT, join_thread->tid);

    join_thread->thread_state = ATHREAD_CREATE_JOINED;

    if(errno != EAGAIN && ret_val == -1){
        return EINVAL;
    }

    if(return_value){
        *(return_value) = (void*)join_thread->return_value;
    }

    _cleanup_thread(join_thread);
    return 0;

}

void athread_exit(void * retval){
    
    /*get the calling threads tcb(thread control block)*/
    athread * current_thread = _wrapper_athread_self();
    
    /*store the return value of the thread routine function*/
    current_thread->return_value = retval;


    longjmp(current_thread->thread_context, 1);
}


int athread_detach(athread_t thread){

   
    athread * target_thread = search_tcb(&task_queue, thread);

    if(target_thread == NULL){
        return ESRCH;
    }

    if(target_thread->thread_state != ATHREAD_CREATE_JOINABLE ){
        return EINVAL;
    }


    //change the state of thread as detached
    target_thread->thread_state = ATHREAD_CREATE_DETACHED;
    
    return 0;
}




/*
 * Delivers the specified signal to the target thread
 * param[1]     - athread Thread handle for the target thread
 * param[2]     - sig_num Signal number
 * Return type  - 0 if successful else returns errono
 */

int athread_kill(athread_t thread, int sig_num) {

    athread * a_thread = search_tcb(&task_queue, thread);

    /*error handling*/
    if(a_thread == NULL){
        return ESRCH;
    }

    if(a_thread->thread_state == ATHREAD_CREATE_JOINED){
        return EINVAL;
    }


    uint32_t thread_group_id;

    /* Get the thread group id */
    thread_group_id = getpid();

    /* Send the kill signal to the target thread */
    if (_tgkill(thread_group_id, a_thread->tid, sig_num) == -1) {

        /* Return error status */
        return EINVAL;
    }

    /* Return success status */
    return THREAD_SUCCESS;
}


/*returns the tls of calling thread using  arch_prctl() system call */
athread_t athread_self(void){
    athread * calling_thread = _wrapper_athread_self();
   
    return calling_thread->tid;

}

/*self blocking operation on thread*/
int athread_yield(void){
    return sched_yield();
}

/*returns 0 if both threads ids are same else returns a non zero value*/
int athread_equal(athread_t thread1, athread_t thread2){
    return (thread1 == thread2);
}





