#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>
#include <signal.h>

#include "athread.h"
#include "mangle.h"
#include "queue.h"
#include "utils.h"
#include "timer.h"
#include "stackmem.h"

/*maximum number of threads*/
#define MAX_NUM_THREADS 64

/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))

/*global defintions*/
static queue *task_queue;
static athread *running_thread;
static int is_initialised;
static athread_t utid = 0;

void _cleanup_handler(void){

    int count = return_count(task_queue);

    while(count > 0 ){
        node * th = dequeue(task_queue);
        athread * thread = th->tcb;

        /*deallocate the stack*/
        _deallocate_stack(thread->stack_base, stack_limit);
     
        thread = NULL;

        free(thread);
        free(th);

        count--;
    }

}

static athread * find_next_runnable_thread(){
    
    if(is_empty(task_queue))
        return NULL;
    
    int count = return_count(task_queue);
    
    while(count > 0){

        node * th = dequeue(task_queue);
        athread * thread = th->tcb;
        switch(thread->thread_state){
            case RUNNABLE:
                        enqueue(task_queue, thread);
                        return thread;
            case WAITING:
            case EXITED:
            case RUNNING:
                        enqueue(task_queue, thread);
                        break;
        }
        count--;
    }
    return NULL;
}

static void scheduler(int signum){

    athread * prev_thread = running_thread;

    athread * next_thread = find_next_runnable_thread();
    
    if(next_thread == NULL){
        timer_enable(&timer);
        return;
    }
    
    /* save context*/
    if(sigsetjmp(running_thread->thread_context, 1) == 1){
        
        sigset_t tmp;
        sigfillset(&tmp);
        sigdelset(&tmp, SIGVTALRM);
        sigprocmask(SIG_UNBLOCK, &tmp, NULL);
        
        /*raise all pending signals*/
        for(int signum = 1; signum < NSIG; signum++){

            if( sigismember(&running_thread->pending_signals, signum) == 1){
                raise(signum);
                sigdelset(&running_thread->pending_signals, signum);
            }
        }
        return;
    }

    

    if(prev_thread->thread_state == RUNNING){
        prev_thread->thread_state = RUNNABLE;
    }

    next_thread->thread_state = RUNNING;
    running_thread = next_thread;

    /*restart the timer*/
    timer_enable(&timer);
 
    /*context switch*/
    siglongjmp(running_thread->thread_context, 1);

}

static void _wrapper_start(void){

    unblock_signal();

    /*call to thread start routine*/
    vptr_t return_value = running_thread->start_routine(running_thread->args);
    
    /*explicit call to athread_exit*/
    athread_exit(return_value);

    return;
    
}

int athread_init(void){

    /*initialize the task queue*/
    task_queue = (queue*)malloc(sizeof(queue));
    qinit(task_queue);

    /*initialize the global definitions*/
    stack_limit = get_stack_limit();
    page_size = get_page_size();
    is_initialised = 1;

    /*set up the cleanup handler*/
    atexit(_cleanup_handler);

    /*create the context of the main thread*/
    athread * main_thread = (athread*)malloc(sizeof(athread));
    if(main_thread == NULL)
        return ENOMEM;

    main_thread->tid = utid++;
    main_thread->start_routine = main_thread->args = main_thread->return_value = NULL;
    main_thread->joining_on = -1;
    main_thread->detachstate = -1;
    main_thread->thread_state = RUNNING;

    /*set the current running thread*/
    running_thread = main_thread;

    /*get context*/
    //sigsetjmp(main_thread->thread_context, 1);

    /*enqueue the thread control block*/
    enqueue(task_queue, main_thread);

    /*set up the signal handler*/
    struct sigaction act;
    
    act.sa_handler = scheduler;
    sigfillset(&act.sa_mask);
    act.sa_flags = SA_RESTART;

    sigaction(SIGVTALRM, &act, NULL);

    /*enable the timer*/
    timer_enable(&timer);

    return 0;
}

int athread_create(athread_t * thread, athread_attr_t * attr, thread_start_t start_routine, void * args){
    
    if(!is_initialised)
        athread_init();
    
    if(return_count(task_queue) == MAX_NUM_THREADS){
        return EAGAIN;
    }

    if(!thread || !start_routine){
        return EFAULT;
    }

    block_signal();

    athread * current = (athread*)malloc(sizeof(athread));
    
    if(current == NULL){
        unblock_signal();
        return EAGAIN;
    }

    current->tid = utid++;
    current->start_routine = start_routine;
    current->args = args;
    current->return_value = NULL;
    current->thread_state = RUNNABLE;
    current->joining_on = -1;
    sigemptyset(&current->pending_signals);
    
    if(attr != NULL){
        current->detachstate = attr->detach_state;
    }
    else{
        current->detachstate = ATHREAD_CREATE_JOINABLE;
    }
    
    /*make context*/
    sigsetjmp(current->thread_context, 1);

    vptr_t stack_base = NULL;
    size_t stack_size;

    if(attr != NULL){
        stack_size = attr->stack_size;
        stack_base = attr->stack_addr;
    }
    else{
        stack_size = stack_limit;
    }

    if(stack_base == NULL){

        stack_base = _stack_allocate(stack_size);
        if(stack_base == NULL){
            unblock_signal();
            return ENOMEM;
        }
    }

    current->stack_base = stack_base;
    vptr_t stack_top = stack_base + stack_size;
    
    /*change the stack pointer to point to top of the stack*/
    current->thread_context[0].__jmpbuf[JB_SP] = i64_ptr_mangle((long int)stack_top - sizeof(long int));

    /*change the program counter to point to start function*/
    current->thread_context[0].__jmpbuf[JB_PC] = i64_ptr_mangle((long int)_wrapper_start);

    /*enqueue thread*/
    enqueue(task_queue, current);

    /*set the thread id*/
    *thread = current->tid;

    unblock_signal();

    return 0;
}

int athread_join(athread_t thread_id, void ** return_value){

    block_signal();

    athread * join_thread = search_tcb(task_queue, thread_id);
    
    /*invalid thread id*/
    if( !join_thread){
        unblock_signal();
        return ESRCH;
    }

    if(join_thread->joining_on == athread_self()){
        unblock_signal();
        return EINVAL;
    }

    /*deadlock error*/
    if(thread_id == athread_self() || running_thread->joining_on == join_thread->tid){
        unblock_signal();
        return EDEADLK;
    }

    if(join_thread->detachstate == ATHREAD_CREATE_EXITED){
       
        if(return_value){
            *(return_value) = NULL;
        }
        unblock_signal();
        return 0;
    }

    /*check if thread is joinable*/
    if(join_thread->detachstate != ATHREAD_CREATE_JOINABLE){
        unblock_signal();
        return EINVAL;
    }

    /*change state*/
    join_thread->joining_on = running_thread->tid;
    running_thread->thread_state = WAITING;
    
    unblock_signal();

    while(join_thread->thread_state != EXITED);

    join_thread->detachstate = ATHREAD_CREATE_JOINED;

    if(return_value){
        *(return_value) = join_thread->return_value;
    }

    
    return 0;    
}

void athread_yield(void){
    raise(SIGVTALRM);
}

athread_t athread_self(void){
    return running_thread->tid;
}

int athread_equal(athread_t thread1, athread_t thread2){
    return thread1 == thread2;
}

void athread_exit(void * retval){

    block_signal();
    
    /*save the thread return value*/
    running_thread->return_value = retval;
  
    /*change the thread state*/
    running_thread->thread_state = EXITED;
    running_thread->detachstate = ATHREAD_CREATE_EXITED;

    if(running_thread->joining_on != -1){
        
        athread * target_thread = search_tcb(task_queue, running_thread->joining_on);
        target_thread->thread_state = RUNNABLE;
    }

    unblock_signal();
    
    /*thread yield*/
    athread_yield();
}

int athread_kill(athread_t thread_id, int signum){
   
    if(signum <= 0 || signum >= NSIG){
        return EINVAL;
    }

    block_signal();

    athread * target_thread = search_tcb(task_queue, thread_id);
    
    if(!target_thread){

        unblock_signal();
        return ESRCH;
    }

    /*if target thread is currently running thread*/
    if(thread_id == athread_self()){

        /*deliver the signal to the target thread*/
        unblock_signal();
        raise(signum);
    }   
    else{

        /*update the pending signal set of the target thread*/
        sigaddset(&target_thread->pending_signals, signum);
    }

    unblock_signal();
    return 0;
   

}


