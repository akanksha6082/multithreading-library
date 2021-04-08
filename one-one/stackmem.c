#include "stackmem.h"


vptr_t _stack_allocate( size_t stack_size) {
    
    vptr_t stack_base;
    _uint page_size;

    /* Get the page size */
    page_size = get_page_size();

    /* Memory map the stack of the requested size plus page size */
    stack_base = mmap(NULL, stack_size + page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    if(stack_base == MAP_FAILED){
        return NULL;
    }

    /* Protect the lowest one page */
    if (mprotect(stack_base, page_size, PROT_NONE) == -1) {

        /* Unmap the previously mapped region */
        munmap(stack_base, stack_size + page_size);

        return NULL;
    }

    /* Return the base of the stack after the lowest page */
    return stack_base + page_size;
}


void _deallocate_stack(vptr_t stack_base, size_t stack_size){
    

    _uint page_size;

    /* Get the page size */
    page_size = get_page_size();

    /* Unmap the allocated stack along with the protection page */
    munmap(stack_base - page_size, stack_size + page_size);

}