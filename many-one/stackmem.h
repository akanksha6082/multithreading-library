#ifndef __STACK_MEM_H
#define __STACK_MEM_H

#include "athread_types.h"
#include "utils.h"

ptr_t _stack_allocate(size_t);
void _deallocate_stack(void *, size_t);

#endif