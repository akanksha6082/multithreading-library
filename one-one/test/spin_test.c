/**
 * 
 * spin lock test
 * The test runs 5 threads each of which individually increments its local varaible
 * and shared variable. To avoid the race condition to access the shared variable and
 * to ensure mutual exlusion between threads spinlocks are used. The mutual exlcusion
 * is ensured by validating the value of shared varaible to sum of individual values of threads.  
 * 
 **/


#include <stdio.h>
#include <stdlib.h>
#include "athread.h"


athread_spinlock_t lock;
long int c=0, c1=0, c2=0, c3=0, c4=0, c5=0, run = 1;


void validate(int result){
    if(result != 0){
        
        fprintf(stdout, "\033[0;31mFATAL ERROR - %d\033[0m\n", result);
        fprintf(stdout, "\033[0;31mTEST FAIL\033[0m\n");
        exit(0);
    }
}

void * thread_1(void * args){
    while(run == 1){
        c1++;
        athread_spin_lock(&lock);
        c++;
        athread_spin_unlock(&lock);
    }
    return NULL;

}

void * thread_2(void * args){

    while(run == 1){
        c2++;
        athread_spin_lock(&lock);
        c++;
        athread_spin_unlock(&lock);
    }
    return NULL;
    
}

void * thread_3(void * args){
    
    while(run == 1){
        c3++;
        athread_spin_lock(&lock);
        c++;
        athread_spin_unlock(&lock);
    }
    return NULL;
    
}

void * thread_4(void * args){

    while(run == 1){
        c4++;
        athread_spin_lock(&lock);
        c++;
        athread_spin_unlock(&lock);
    }
    return NULL;
    
}

void * thread_5(void * args){
    
    while(run == 1){
        c5++;
        athread_spin_lock(&lock);
        c++;
        athread_spin_unlock(&lock);
    }
    return NULL;
    
}

int main(int argc, char ** argv){

    if(argc != 2){
        fprintf(stdout, "invalid arguments, aborting %s\n", argv[0]);
        return -1;
    }

    athread_init();

    athread_spin_init(&lock);

    athread_t t1, t2, t3, t4, t5;
    athread_attr_t attr;
    athread_attr_init(&attr);

    validate(athread_create(&t1, &attr, thread_1, NULL));
    validate(athread_create(&t2, &attr, thread_2, NULL));
    validate(athread_create(&t3, &attr, thread_3, NULL));
    validate(athread_create(&t4, &attr, thread_4, NULL));
    validate(athread_create(&t5, &attr, thread_5, NULL));

    fprintf(stdout,"created 5 threads\n\n");
    fprintf(stdout,"running threads for %d seconds\n\n", atoi(argv[1]));


    sleep(atoi(argv[1]));
    run = 0;


    validate(athread_join(t1, NULL));
    validate(athread_join(t2, NULL));
    validate(athread_join(t3, NULL));
    validate(athread_join(t4, NULL));
    validate(athread_join(t5, NULL));

    fprintf(stdout, "joined on all 5 threads\n\n");

    fprintf(stdout, "Thread 1              = %ld\n", c1);
    fprintf(stdout, "Thread 2              = %ld\n", c2);
    fprintf(stdout, "Thread 3              = %ld\n", c3);
    fprintf(stdout, "Thread 4              = %ld\n", c4);
    fprintf(stdout, "Thread 5              = %ld\n", c5);
    fprintf(stdout, "sum of thread count   = %ld\n", c1 + c2 + c3 + c4 + c5);
    fprintf(stdout, "shared varaible count = %ld\n\n", c);

    if(c == c1 + c2 + c3 + c4 + c5){
        fprintf(stdout, "\033[0;32mTEST PASS\033[0m\n");
    }
    else{
        fprintf(stdout, "\033[0;31mTEST FAIL\033[0m\n");
    }

    athread_spin_destroy(&lock);
    return 0;

}