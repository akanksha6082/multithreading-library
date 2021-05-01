#ifndef __STACK_MEM_H
#define __STACK_MEM_H

#include "athread_types.h"


vptr_t _stack_allocate(size_t);
void _deallocate_stack(void *, size_t);

#endif