#ifndef __ATHREAD_H
#define __ATRHEAD_H

#include "athread_types.h"
#include "queue.h"
#include "utils.h"
#include "stackmem.h"
#include "athread_attr.h"

static size_t stack_limit;
static _uint max_allowed_threads;
static size_t page_size;
static queue task_queue;


/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))


/*Thread fucntions*/


int athread_init(void);

int athread_create( athread_t *thread, athread_attr_t *attr, thread_start_t start_routine, ptr_t args);

int athread_join(athread_t thread_id, void ** return_value );

void athread_exit(void * retval);

int athread_yield(void);

int athread_equal(athread_t thread1, athread_t thread2);

athread_t athread_self(void);

int athread_detach(athread_t thread);


#endif