#include "athread.h"

int athread_init(){

    qinit(&ready_queue); qinit(&wait_queue); qinit(&exit_queue);
    max_allowed_threads = get_threads_limit();
    stack_limit = get_stack_limit();
    page_size = get_page_size();
    is_initialised = 1;

    return 0;
}


