#ifndef __MANGLE_H
#define __MANGLE_H

#define JB_SP 6
#define JB_PC 7

/* @code snippet credit - http://sites.cs.ucsb.edu/~chris/teaching/cs170/projects/proj2.html */

static long int i64_ptr_mangle(long int p){
    long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%rax"
    );
    return ret;
}

#endif
