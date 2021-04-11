#ifndef __ATHREAD_H
#define __ATRHEAD_H

#include "athread_types.h"

/* Size of thread control block */
#define TCB_SIZE (sizeof(athread))


/*Thread fucntions*/

/*initialization routine*/
int athread_init(void);


/*  
 *  param[1] - pointer to athread_t variable to store thread_id of new created thread.
 *  param[2] - pointer to a thread attribute object used to set thread attributes.
 *  param[3] - Pointer to a thread start routine. 
 *  param[4] - arguments to start routine.
 *  return type - 0 if succesful else error number
 */ 

int athread_create( athread_t *thread, athread_attr_t *attr, thread_start_t start_routine, vptr_t args);

/*
 * param[1] - target thread id
 * param[2] - pointer to location to store the exit status of target thread
 * retrun value - 0 if successful else error number
 */

int athread_join(athread_t thread_id, void ** return_value );

/*
 * param - exit status of calling thread
 * return value - function doesnot return
 */

void athread_exit(void * retval);

/*
 * param[1] - thread id
 * param[2] - thread id
 * return value - zero if threads are equal else non-zero value is returned
 */
int athread_equal(athread_t thread1, athread_t thread2);


/* returns the thread id of calling thread*/
athread_t athread_self(void);


/*self-blocking gives up the cpu*/
int athread_yield(void);

/*
 * param - target thread id
 * return value - zero if succesful else erro number is returned.
 */

int athread_detach(athread_t thread);

/*
 * delivers the specified signal to the target thread
 * param[1]     - athread Thread handle for the target thread
 * param[2]     - sig_num Signal number
 * return type  - 0 if successful else returns error number.
 */

int athread_kill(athread_t thread, int sig);


/*spinlock functions*/


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


/*mutex functions*/


/*
 * param[1]     - pointer to the mutex instance
 * return Value - thread return status
 * intializes the lock instance
 */

int athread_mutex_init(athread_mutex_t * mutex);

/*
 * params       - mutex lock pointer
 * return Value - thread return status
 * mutex sleep lock implementation
 */

int athread_mutex_lock(athread_mutex_t * mutex);

/*
 * params : mutex lock pointer
 * return Value : thread return status
 * releases the lock
 */

int athread_mutex_unlock(athread_mutex_t * mutex);

/*
 * params       - mutex lock pointer
 * return Value - thread return status
 * destroys the lock instance
 */

int athread_mutex_destroy(athread_mutex_t * mutex);


#endif