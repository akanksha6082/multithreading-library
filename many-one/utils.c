#include "utils.h"

_uint get_stack_limit(){
    
    struct rlimit rlimit;
    int resource = RLIMIT_STACK;
    getrlimit(resource, &rlimit);
    return rlimit.rlim_cur;
}

_uint get_page_size(){
    
    return sysconf(_SC_PAGESIZE);
}
