#include <sys/resource.h>
#include <unistd.h>


#include "utils.h"

/*get stack limit*/
_uint get_stack_limit(){
    
    struct rlimit rlimit;
    int resource = RLIMIT_STACK;
    getrlimit(resource, &rlimit);
    return rlimit.rlim_cur;
}

/*get page size*/
_uint get_page_size(){
    
    return sysconf(_SC_PAGESIZE);
}
