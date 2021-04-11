#include<stdlib.h>
#include<errno.h>

#include "athread_attr.h"
#include "utils.h"

#define ATHREAD_STACK_MIN 16384


int athread_attr_init(athread_attr_t * attr){

    if(attr == NULL)
        return EINVAL;

	
    attr->detach_state = ATHREAD_CREATE_JOINABLE;
    attr->stack_size = get_stack_limit();
    attr->stack_addr = NULL;

    return 0;
}

int athread_attr_destroy(athread_attr_t * attr){
    
    if(attr == NULL)
        return EINVAL;

    free(attr);

    return 0;
}

int athread_attr_setdetachstate(athread_attr_t *attr, int detachstate){
    if(attr){
        if(detachstate < 0 && detachstate > 2){
            return EINVAL;
        }
        attr->detach_state = detachstate;
        return 0;
    }
    return EINVAL;
}

int athread_attr_getdetachstate(athread_attr_t *attr, int * detachstate){
    if(attr){
        *(detachstate) = attr->detach_state;
        return 0;
    }
    return EINVAL;
}

int athread_attr_setstacksize(athread_attr_t *attr, size_t stacksize){
   if(attr){
        
        if(stacksize < ATHREAD_STACK_MIN)
            return EINVAL;
            
        if((float)(stacksize / get_page_size()) == (float)0  && stacksize < get_stack_limit()){
            attr->stack_size = stacksize;
            return 0;
        }
   }
   return EINVAL;
}

int athread_attr_getstacksize(athread_attr_t *attr, size_t * stacksize){
    
    if(attr){
        *(stacksize) = attr->detach_state;
        return 0;
    }
    return EINVAL;
}


int athread_attr_setstack(athread_attr_t *attr, void * stackaddr, size_t stacksize){
    if(attr){
        if(stacksize < ATHREAD_STACK_MIN)
            return EINVAL;
        attr->stack_addr = stackaddr;
        attr->stack_size = stacksize;

    }
    return EINVAL;
}

int athread_attr_getstack(athread_attr_t *attr, void ** stackaddr, size_t *stacksize){
    if(attr){

        *(stacksize) = attr->stack_size;
        *(stackaddr) = attr->stack_addr;
        return 0;
    }
    return EINVAL;
}