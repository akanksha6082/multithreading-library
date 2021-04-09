#define _GNU_SOURCE
#include "athread.h"
#include "stackmem.h"

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

/*
 * sends a signal to a thread in the thread group
 * param[1]     - tgid Thread group id
 * param[2]     - tid Thread id
 * param[3]     - sig Signal number
 * return type  - Integer status
 */

static inline int _tgkill(int tgid, int tid, int sig) {

    /* system call wrapper to invoke the call */
    return syscall(SYS_tgkill, tgid, tid, sig);
}

static void _cleanup_handler(void){

    while(!is_empty(&task_queue)){
        
        node * thread = dequeue(&task_queue);

        _deallocate_stack(thread->tcb->stack_base, thread->tcb->stack_size);

        free(thread->tcb);
        thread->tcb = NULL;
        free(thread);
    }
}

/*initization routine handler*/
int athread_init(){

    /*intialize the thread queue data structure*/
    qinit(&task_queue);

    
    /*get system allowed max threads*/
    max_allowed_threads = get_threads_limit();

    
    /*get stack limit per thread*/
    stack_limit = get_stack_limit();

    
    /*get page size*/
    page_size = get_page_size();

    
    /*set initialization variable*/
    is_initialised = 1;

    /*set up the clean up handler*/
    atexit(_cleanup_handler);

    return 0;
}

/*change this later*/

int _wrapper_start(void * argument){
    
    athread * thread_block;
    thread_block = (athread *)argument;


    /*returned to the called location via long jump*/
    if(setjmp(thread_block->thread_context) == 0){
        thread_block->return_value = thread_block->start_routine(thread_block->args);
        
    }

    thread_block->thread_state = ATHREAD_CREATE_EXITED;
    
    return 0;
}

/*thread create*/
int athread_create( athread_t *thread, athread_attr_t *attr, thread_start_t start_routine, vptr_t args){
    
    /*check for intialization*/
    if(!is_initialised){
        athread_init();
    }

    /*check for thread limit*/
    if(return_qcount(&task_queue) == max_allowed_threads){
        return EAGAIN;
    }

    /*check for errors*/
    if(!thread || !start_routine){
        return EAGAIN;
    }


    /*allocation of thread control block*/ 
    athread * thread_block = (athread*)malloc(TCB_SIZE);

    if (!thread_block) {
        return ENOMEM;
    }

    vptr_t stack_top;

    thread_block->args = args;
    thread_block->start_routine = start_routine;
    thread_block->return_value = 0;
    thread_block->joining_on = -1;

    /*check for customized attribute values else set the default thread attributes*/
    if(attr){
        
        thread_block->thread_state = attr->detach_state ? attr->detach_state : ATHREAD_CREATE_JOINABLE;
        thread_block->stack_size = attr->stack_size ? attr->stack_size : stack_limit;
        thread_block->stack_base = attr->stack_addr ? attr->stack_addr : NULL ;
    }
    else{
        
        thread_block->thread_state = ATHREAD_CREATE_JOINABLE;
        thread_block->stack_size = stack_limit;
        thread_block->stack_base = NULL ;

    }
   

    if(thread_block->stack_base == NULL){
        
        /*allocate the stack to thread*/
        thread_block->stack_base = _stack_allocate(thread_block->stack_size);
        if(thread_block->stack_base == NULL){
            return ENOMEM;
        }
    }

    /* get the stack top */
    stack_top = thread_block->stack_base + thread_block->stack_size;


     /*thread clone */
    thread_block->tid = clone(_wrapper_start,
                                    stack_top,
                                    CLONE_VM | CLONE_FS | CLONE_FILES |
                                    CLONE_SIGHAND | CLONE_THREAD |
                                    CLONE_SYSVSEM | CLONE_PARENT_SETTID |
                                    CLONE_CHILD_CLEARTID,
                                    thread_block,
                                    &thread_block->futex,
                                    NULL,
                                    &thread_block->futex);

    /*check for errors*/                                
    if(thread_block->tid == -1){
        
        (thread) = NULL;
        _deallocate_stack(thread_block->stack_base, thread_block->stack_size);
        free(thread_block);

        return EINVAL;
    }

    /*enqueue thread*/
    enqueue(&task_queue, thread_block);

    /*set the thread id*/
    *thread = thread_block->tid;

    return 0;

}

/*thread join - wait for target thread to terminate*/
int athread_join(athread_t thread_id, void ** return_value){
    
    /*search for target thread*/
    athread * join_thread = search_tcb(&task_queue, thread_id);

    /*check for erros*/
    if(join_thread == NULL){
        return ESRCH;
    }

    /*check for the deadlock condition*/
    if(join_thread->joining_on  == athread_self() || thread_id == athread_self()){
        return EDEADLK;
    }

    /*check for thread state*/
    if(join_thread->thread_state == ATHREAD_CREATE_DETACHED || join_thread->thread_state == ATHREAD_CREATE_JOINED){
        return EINVAL;
    }

    /* wait on the thread's futex word -- returns if the value changes */
    /* kernel checks if the value at addr is the same as val; if so, it then blocks the calling thread*/
    _uint ret_val = _futex(&join_thread->futex, FUTEX_WAIT, join_thread->tid);

    /*change the satus of the target thread*/
    join_thread->thread_state = ATHREAD_CREATE_JOINED;


    if(errno != EAGAIN && ret_val == -1){
        return EINVAL;
    }

    /*set the target thread's exit status at the lcoation pointed by return_value*/
    if(return_value){
        int ptr = *(int*)(join_thread->return_value);
        int *p = &ptr;
        void * ret = (void*)p;
        *(return_value) = ret;
    }

    return 0;

}

/*thread exit - terminates the thread*/
void athread_exit(void * retval){
    
    /*get the calling thread thread-id*/
    athread_t tid = athread_self();

    /*get the calling threads tcb(thread control block)*/
    athread * current_thread = search_tcb(&task_queue, tid);

    /*store the return value of the thread routine function*/
    void * return_value = &retval;
    current_thread->return_value = return_value;
   
    longjmp(current_thread->thread_context, 1);
}

/*thread detach - target thread state chnaged to detached*/
int athread_detach(athread_t thread){

   
    athread * target_thread = search_tcb(&task_queue, thread);

    /*check for errors*/
    if(target_thread == NULL){
        return ESRCH;
    }

    if(target_thread->thread_state != ATHREAD_CREATE_JOINABLE ){
        return EINVAL;
    }


    /*change the state of thread as detached*/
    target_thread->thread_state = ATHREAD_CREATE_DETACHED;
    
    return 0;
}

/*thread kill - sends signal to target thread*/
int athread_kill(athread_t thread, int sig_num) {

    athread * a_thread = search_tcb(&task_queue, thread);

    /*check for errors*/
    if(a_thread == NULL){
        return ESRCH;
    }

    if(a_thread->thread_state == ATHREAD_CREATE_JOINED || a_thread->thread_state == ATHREAD_CREATE_EXITED){
        return EINVAL;
    }

    athread_t thread_group_id;

    /* get the thread group id */
    thread_group_id = getpid();

    /* send the kill signal to the target thread */
    if (_tgkill(thread_group_id, a_thread->tid, sig_num) == -1) {

        /* return error status */
        return EINVAL;
    }

    /* return success status */
    return 0;
}


/*thread self - returns the thread id of the calling thread*/
inline athread_t athread_self(void){
    return syscall(SYS_gettid);
}

/*thread yield - self blocking operation on thread*/
inline int athread_yield(void){
    return sched_yield();
}

/*thread equal - check if two threads are equal*/
inline int athread_equal(athread_t thread1, athread_t thread2){
    return (thread1 == thread2);
}





