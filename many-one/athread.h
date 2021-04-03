#ifndef __ATHREAD_H
#define __ATHREAD_H

#include "athread_types.h"
#include "queue.h"
#include "utils.h"
#include "timer.h"
#include "stackmem.h"

#define MAX_NUM_THREADS 64
#define TIMER_INTERRUPT 10000

/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))

typedef struct itimerval athread_timer_t;

/*global defintions*/
static size_t stack_limit;
static _uint max_allowed_threads;
static size_t page_size;
static queue *task_queue;
static athread * running_thread;
static int is_initialised;
static athread_t utid = 0;
static athread_timer_t timer;
sigset_t sigset;


/*thread functions*/

int athread_init(void);

int athread_create(athread_t * thread, void*(*start_routine)(void *), void * args);

int athread_equal(athread_t thread1, athread_t thread2);

int athread_join(athread_t thread_id, void ** return_value);

void athread_yield();


#endif