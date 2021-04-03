#include "athread.h"

int athread_init(){

    struct sigaction action;
    
    /*intialization of globals*/
    task_queue = (queue *)malloc(sizeof(queue));
    qinit(task_queue);
    max_allowed_threads = MAX_NUM_THREADS;
    stack_limit = get_stack_limit();
    page_size = get_page_size();
    is_initialised = 1;

    /*main thread context*/
    athread * main_thread = (athread *)malloc(sizeof(athread));
    if(main_thread == NULL)
        return -1;

    main_thread->tid = utid++;
    main_thread->start_routine = main_thread->args =  main_thread->return_value = NULL;
    main_thread->thread_context = (ucontext_t *)malloc(sizeof(ucontext_t));
    main_thread->waitid = NULL;
    main_thread->detachstate = -1;

    memset(main_thread->thread_context, '\0', sizeof(ucontext_t));

    if(getcontext(main_thread->thread_context) == -1)
        return -1;
    
    /*set state of main thread*/
    main_thread->thread_state = RUNNING;

    /*set main thread to currently running thread*/
    running_thread = main_thread;

    /*add main_thread to queue*/
    enqueue(&task_queue, main_thread);


    /*set up the signal mask*/
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);


    /*install signal handler*/
    memset(&action, '\0', sizeof(action));
    action.sa_handler = &scheduler;

    if (sigaction(SIGALRM, &action, NULL) < 0)
        return -1;

    /*enable the timer interrupt*/
    enable_interrupt();

    return 0;
}

void wrapper_start(void *(*start_routine)(void *), void * args, athread * thread){
    
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    thread->return_value = start_routine(args);

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    return;

}


int athread_create(athread_t * tid, thread_start_t start_routine, void * args){

    /*check if intialisation is done*/
    if(!is_initialised){
        athread_init();
    }

    /*check for errors */
    if(return_qcount(task_queue) == max_allowed_threads){
        return EAGAIN;
    }

    if(!tid || !start_routine){
        return EAGAIN;
    }

    sigprocmask(SIG_BLOCK, &sigset, NULL);
    
    /* set up context of thread */
    athread * thread = (athread *)malloc(sizeof(athread));
    if(thread == NULL)
        return -1;

    thread->tid = utid++;
    thread->args = args;
    thread->start_routine = start_routine; 
    thread->return_value = NULL;
    thread->waitid = NULL;
    thread->detachstate = ATHREAD_CREATE_JOINABLE;
    thread->thread_state = RUNNABLE;
    thread->thread_context = (ucontext_t*)malloc(sizeof(ucontext_t));
    
    memset(thread->thread_context, '\0', sizeof(ucontext_t));


    if (getcontext(thread->thread_context) == -1)
        return -1;

    
    /*allocate the stack*/
    ptr_t stack_base = _stack_allocate(stack_limit);
    if(stack_base == NULL){
            return ENOMEM;
    }

    thread->thread_context->uc_stack.ss_sp = stack_base;
    thread->thread_context->uc_stack.ss_size = stack_limit;
    thread->thread_context->uc_flags = 0;
    thread->thread_context->uc_link = NULL;
    
    makecontext(thread->thread_context, (void (*)(void)) wrapper_start, 3, start_routine, thread->args, thread);
    
    /*add thread block to queue*/
    enqueue(&task_queue, thread);

    *tid = thread->tid;

    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    return 0;
}

static athread * find_next_runnable_thread(){
    
    if(is_empty(task_queue))
        return NULL;
    
    int count  = return_count(task_queue);
    while(count > 0){
        athread * thread = dequeue(task_queue);
        switch (thread->thread_state){
            case RUNNABLE:
                        enqueue(task_queue, thread);
                        return thread;
            case WAITING:
            case EXITED:
                        enqueue(task_queue, thread);
                        break;
        }
        count--;
    }
    return NULL;
}

static void scheduler(int signum){
    
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    /*save the current thread context*/
    athread * prev_thread =  running_thread;

    athread * next_thread = find_next_runnable_thread();

    /*continue with execution*/
    if(next_thread == NULL)
        return;
    
    prev_thread->thread_state = RUNNABLE;
    next_thread->thread_state = RUNNING;
    running_thread = next_thread;

    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    swapcontext(prev_thread->thread_context, next_thread->thread_context);

    return;

}

int athread_equal(athread_t thread1, athread_t thread2)
    return (thread1 == thread2);

void athread_yield(){
    
    raise(SIGVTALRM);
    return;
}

athread_t athread_self(void){
    return running_thread->tid;
}

int athread_join(athread_t thread_id,  void ** return_value){
    
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    
    athread * join_thread = search_tcb(task_queue, thread_id);
    
    /*check for errors*/
    if(join_thread == NULL){
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        return ESRCH;
    }

    /*check for deadlock*/
    if(thread_id == running_thread->tid){
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        return EDEADLK;

    }
        
    
    /*check if thread is joinable*/
    if(join_thread->detachstate == ATHREAD_CREATE_JOINED || join_thread->detachstate == ATHREAD_CREATE_DETACH){
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        return EINVAL;
    }
        
    /*change the state*/
    join_thread->detachstate = ATHREAD_CREATE_JOINED;
    running_thread->thread_state = WAITING;


    /*enable timer interrupt*/
    sigprocmask(SIG_UNBLOCK, &sigset, NULL);

    /*wait for thread to terminate*/
    while(join_thread->thread_state != EXITED);

    if(return_value){
        *(return_value) = (void*)join_thread->return_value;
    }

    return 0;
}

