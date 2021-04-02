#ifndef __ATHREAD_H
#define __ATHREAD_H

#include "athread_types.h"
#include "queue.h"

/*global defintions*/
static size_t stack_limit;
static _uint max_allowed_threads;
static size_t page_size;
static int is_initialised;
static queue ready_queue, wait_queue, exit_queue;


/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))


/*thread functions*/

int athread_init(void);

int athread_create(athread_t * thread, void*(*start_routine)(void *), void * args);


#endif