#ifndef __ATHREAD_H
#define __ATHREAD_H

#include "athread_types.h"
#include "queue.h"
#include "utils.h"
#include "timer.h"
#include "stackmem.h"

#define MAX_NUM_THREADS 64


/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))



/*global defintions*/


static queue *task_queue;
static athread * running_thread;
static int is_initialised;
static athread_t utid = 0;
sigset_t sigset;


/*thread functions*/

int athread_init(void);

int athread_create(athread_t * thread, void*(*start_routine)(void *), void * args);

int athread_equal(athread_t thread1, athread_t thread2);

int athread_join(athread_t thread_id, void ** return_value);

athread_t athread_self(void);

void athread_yield();

void athread_exit(void * retval);


#endif