#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <syscall.h>

#include "athread_kill.h"

/**
 * Sends a signal to a thread in the thread group
 * param[1]     - tgid Thread group id
 * param[2]     - tid Thread id
 * param[3]     - sig Signal number
 * Return type  - Integer status
 */
int _tgkill(int tgid, int tid, int sig) {

    /* Use the system call wrapper to invoke the call */
    return syscall(SYS_tgkill, tgid, tid, sig);
}

/*
 * Delivers the specified signal to the target thread
 * param[1]     - athread Thread handle for the target thread
 * param[2]     - sig_num Signal number
 * Return type  - Thread return status
 */
ThreadReturn athread_kill(thread a_thread, int sig_num) {

    uint32_t thread_group_id;

    /* Get the thread group id */
    thread_group_id = getpid();

    /* Send the kill signal to the target thread */
    if (_tgkill(thread_group_id, a_thread->tid, sig_num) == -1) {

        /* Return error status */
        return THREAD_FAIL;
    }

    /* Return success status */
    return THREAD_SUCCESS;
}