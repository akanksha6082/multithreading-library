#include <stdio.h>
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

sigset_t sched_sigblock;

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

void scheduler(int signum){


    block_signal();


    athread * prev_thread = running_thread;
    //printf("prev thread was : %d\n", prev_thread->tid);

    athread * next_thread = find_next_runnable_thread();
    

    if(next_thread == NULL){
        timer_enable(&timer);
        return;
    }

    //printf("next thread was : %d\n", next_thread->tid);

    /* save context*/
    if(sigsetjmp(running_thread->thread_context, 0) == 1){

        sigprocmask(SIG_UNBLOCK, &sched_sigblock, NULL);
        /*raise all pending signals*/
        for(int signum = 1; signum < NSIG; signum++){

            if( sigismember(&prev_thread->pending_signals, signum) == 1){
                raise(signum);
                sigdelset(&prev_thread->pending_signals, signum);
            }
        }
        sigprocmask(SIG_BLOCK, &sched_sigblock, NULL);
        return;
    }

    if(prev_thread->thread_state == RUNNING){
        prev_thread->thread_state = RUNNABLE;
    }

    next_thread->thread_state = RUNNING;
    running_thread = next_thread;


    unblock_signal();

    /*restart the timer*/
    timer_enable(&timer);
    
    /*context switch*/
    siglongjmp(running_thread->thread_context, 0);

}

static void _wrapper_start(void){
    
    unblock_signal();

    sigset_t signals;
    sigfillset(&signals);
    sigprocmask(SIG_UNBLOCK, &signals, NULL);

    /*call to thread start routine*/
    vptr_t * return_value = running_thread->start_routine(running_thread->args);
    
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
    sigsetjmp(main_thread->thread_context, 0);

    /*enqueue the thread control block*/
    enqueue(task_queue, main_thread);

    /*set up the signal handler*/
    struct sigaction act;

    // sigset_t sched_sigblock;
    sigfillset(&sched_sigblock);
    
    act.sa_handler = scheduler;
    act.sa_mask = sched_sigblock;
    act.sa_flags = SA_RESTART;

    sigaction(SIGVTALRM, &act, NULL);

    /*enable the timer*/
    timer_enable(&timer);

    return 0;
}

int athread_create(athread_t * thread, thread_start_t start_routine, void * args){
    
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
        return ENOMEM;
    }

    current->tid = utid++;
    current->start_routine = start_routine;
    current->args = args;
    current->return_value = NULL;
    current->detachstate = ATHREAD_CREATE_JOINABLE;
    current->thread_state = RUNNABLE;
    current->joining_on = -1;
    sigemptyset(&current->pending_signals);

    /*make context*/
    setjmp(current->thread_context);

    /*allocate thread stack*/
    vptr_t stack_base = _stack_allocate(stack_limit);
    
    if(stack_base == NULL){
        unblock_signal();
        return ENOMEM;
    }

    current->stack_base = stack_base;
    vptr_t stack_top = stack_base + stack_limit;
    
    
    /*make thread context*/
    sigsetjmp(current->thread_context, 1);
    
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

    /*deadlock error*/
    if(thread_id == athread_self() || join_thread->joining_on == athread_self()){
        unblock_signal();
        return EDEADLK;
    }

    /*check if thread is joinable*/
    if(join_thread->detachstate == ATHREAD_CREATE_JOINED || join_thread->detachstate == ATHREAD_CREATE_DETACHED){
        unblock_signal();
        return EINVAL;
    }

    /*change state*/
    running_thread->thread_state = WAITING;
    join_thread->joining_on = running_thread->tid;

    unblock_signal();

    while(join_thread->thread_state != EXITED);

    join_thread->detachstate = ATHREAD_CREATE_JOINED;

    if(return_value){

        int ptr = *(int*)(join_thread->return_value);
        int *p = &ptr;
        void * ret = (void*)p;
        *(return_value) = ret;
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
    running_thread->thread_state = ATHREAD_CREATE_EXITED;

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


