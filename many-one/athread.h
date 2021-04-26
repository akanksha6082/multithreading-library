#ifndef __ATHREAD_H
#define __ATHREAD_H

#include "athread_types.h"
#include "athread_attr.h"

/*thread functions*/

int athread_init(void);

int athread_create(athread_t * thread, athread_attr_t * attr, void*(*start_routine)(void *), void * args);

int athread_equal(athread_t thread1, athread_t thread2);

int athread_join(athread_t thread_id, void ** return_value);

athread_t athread_self(void);

void athread_yield(void);

void athread_exit(void * retval);

int athread_kill(athread_t thread_id, int signum);

/*spin lock functions*/


/*
 * param[1]     - pointer to the spinlock instance
 * return Value - thread return status
 * intializes the lock instance
 */

int athread_spin_init(athread_spinlock_t *spinlock);

/*
 * params       - spinlock pointer
 * return Value - thread return status
 * performs busy wait to acquire lock
 */

int athread_spin_lock(athread_spinlock_t *spinlock);

/*
 * param        - spinlock pointer to the spinlock instance
 * return Value - thread return status
 * releases the lock
 */

int athread_spin_unlock(athread_spinlock_t *spinlock);

/*
 * param        - spinlock pointer to the spinlock instance
 * return Value - thread return status
 * destroys the lock instance
 */

int athread_spin_destroy(athread_spinlock_t *spinlock);



#endif