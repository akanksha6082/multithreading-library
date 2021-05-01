#ifndef __ATHREAD_H
#define __ATHREAD_H

#include "athread_types.h"
#include "athread_attr.h"

/*thread functions*/

/*initialization routine*/
int athread_init(void);


/*  
 *  param[1] - pointer to athread_t variable to store thread_id of new created thread.
 *  param[2] - pointer to a thread attribute object used to set thread attributes.
 *  param[3] - Pointer to a thread start routine. 
 *  param[4] - arguments to start routine.
 *  return type - 0 if succesful else error number
 */ 

int athread_create(athread_t * thread, athread_attr_t * attr, void*(*start_routine)(void *), void * args);

/*
 * param[1] - thread id
 * param[2] - thread id
 * return value - zero if threads are equal else non-zero value is returned
 */
int athread_equal(athread_t thread1, athread_t thread2);

/*
 * param[1] - target thread id
 * param[2] - pointer to location to store the exit status of target thread
 * retrun value - 0 if successful else error number
 */
int athread_join(athread_t thread_id, void ** return_value);

/* returns the thread id of calling thread*/
athread_t athread_self(void);

/*self-blocking gives up the cpu*/
void athread_yield(void);

/*
 * param - exit status of calling thread
 * return value - function doesnot return
 */

void athread_exit(void * retval);

/*
 * delivers the specified signal to the target thread
 * param[1]     - athread Thread handle for the target thread
 * param[2]     - sig_num Signal number
 * return type  - 0 if successful else returns error number.
 */
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