/*
 * validates the standarad interface provided by library
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "athread.h"

//jiska expected result is zero
void CHECK_PASS(int return_value){
    if(return_value == 0){
        fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n"); 
    }
    else{
        fprintf(stderr, "\nERROR: %s (%d)\n\n", strerror(return_value), return_value);   
        fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
    }
}      
                                                                    
//jiska expected result is error
void CHECK_FAIL(int return_value){
    if(return_value != 0){
        fprintf(stderr, "\nERROR: %s (%d)\n\n", strerror(return_value), return_value);   
        fprintf(stdout, "\033[0;32mTEST PASS\033[0m\n\n");
    }else{
        fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
    }
}

void CHECK(int return_value){
    if(return_value != 0){
        fprintf(stderr, "\nERROR: %s (%d)\n\n", strerror(return_value), return_value);
    }
}



void * thread_1(void * args){
    int a = 100;
    void * ptr = &a;
    return ptr;
}

void * thread_2(void* args){
    int a = 200;
    void * ptr = &a;
    sleep(3);
    return ptr;
}

/*thread returns using return keyword*/
void * ret_thread(void* args){
    int a = 500;
    void * ptr = &a;
    printf("Thread return using return keyword\n\n");
    sleep(5);
    return ptr;
}

/*thread returns using athread_exit() interface */
void * exit_thread(void * args) {
    int a = 700;
    void * ptr = &a;
    printf("Thread return using athread_exit api\n\n");
    
    athread_exit(ptr);
}

void exit_thread_sub_func(int a){

    int ret = a;
    void * ptr = &ret;
    printf("thread exit from sub function\n");
    athread_exit(ptr);
}

/*thread return via sub function*/
void * exit_thread_sub(void * args) {
    
    int a = 800;
    printf("call to sub function\n");
    exit_thread_sub_func(a);
    return NULL;
}


void handler(int signum){
    printf("thread recieved SIGINT(%d) signal\n", signum);
}

void * signal_thread(void * args){

    signal(SIGINT, handler);
    while(1);
    
}

int main(int argc, char ** argv){
    
    athread_init();

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    ATTRIBUTE HANDLING\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 : handling detach state of thread\033[0m\n");

    {
        athread_t tid;
        athread_attr_t attr;
        athread_attr_init(&attr);
        int detachedstate;

        CHECK(athread_attr_getdetachstate(&attr, &detachedstate));

        printf("\nchanging thread state to detached(0)\n");

        CHECK(athread_attr_setdetachstate(&attr, ATHREAD_CREATE_DETACHED));
        athread_attr_getdetachstate(&attr, &detachedstate);
        
        printf("expected value of detach state 0\n");
        printf("actual value of detach state  %d\n", detachedstate);

        if(detachedstate == 0){
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n");
        }
        else{
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");  
        }
    }
    
    fprintf(stdout, "\033[1;34mcase 2 : handling stack for thread\033[0m\n\n");
    
    {
        athread_attr_t attr;
        athread_attr_init(&attr);
            
        void * stack = malloc(1024 * 20);
        printf("set stack address   - %p\n", stack);
        printf("set stack size      - %ld\n\n", (size_t)1024 * 20);
        
        CHECK(athread_attr_setstack(&attr, stack, (size_t)1024 * 20));
        
        void * get_stack_addr; size_t get_stack_size;
        
        CHECK(athread_attr_getstack(&attr, &get_stack_addr, &get_stack_size ));
        
        printf("new stack address   - %p\n", get_stack_addr);
        printf("new stack size      - %ld\n", get_stack_size);

        if((get_stack_size == (size_t)1024 * 20) && stack == get_stack_addr){
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n");
        }
        else{
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }
    }

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    THREAD CREATE\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 : creating thread with invalid thread start routine\033[0m\n");

    athread_t tid;
    CHECK_FAIL(athread_create(&tid, NULL, NULL, NULL));

    fprintf(stdout, "\033[1;34mcase 2 : creating thread with invalid thread descriptor\033[0m\n");

    CHECK_FAIL(athread_create(NULL, NULL, thread_1, NULL));

    fprintf(stdout, "\033[1;34mcase 3 : creating thread with deafault attribute values \033[0m\n");
    {
        athread_attr_t attr;
        CHECK(athread_attr_init(&attr));
        CHECK_PASS(athread_create(&tid, &attr, thread_1, NULL));
        CHECK(athread_join(tid, NULL));
    }
   
    fprintf(stdout, "\033[1;34mcase 4 : creating detached thread \033[0m\n");
    {
        athread_t tid;
        athread_attr_t attr;
        CHECK(athread_attr_init(&attr));
        CHECK(athread_attr_setdetachstate(&attr, ATHREAD_CREATE_DETACHED));
        CHECK_PASS(athread_create(&tid, &attr, thread_1, NULL));
    }

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    THREAD JOIN\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 : Thread join with invalid thread descriptor\033[0m\n");

    {
        athread_t tid;
        CHECK(athread_create(&tid, NULL, thread_1, NULL));
        CHECK_FAIL(athread_join(-1, NULL));
    }

    fprintf(stdout, "\n\033[1;34mcase 2 : thread join on already joined thread\033[0m\n");
    {
        athread_t tid;
        CHECK(athread_create(&tid, NULL, thread_1, NULL));
        CHECK(athread_join(tid, NULL));
        CHECK_FAIL(athread_join(tid, NULL));   
    }

    fprintf(stdout, "\n\033[1;34mcase 3 : thread join on detached thread\033[0m\n");
    {
        athread_t tid;
        athread_attr_t attr;
        CHECK(athread_attr_init(&attr));
        CHECK(athread_attr_setdetachstate(&attr, ATHREAD_CREATE_DETACHED));
        CHECK(athread_create(&tid, &attr, thread_1, NULL));
        CHECK_FAIL(athread_join(tid, NULL));

    }

    fprintf(stdout, "\n\033[1;34mcase 4 : thread join on multiple thread\033[0m\n\n");
    {
        athread_t tid[3];
        for(int i=0; i<3; i++){
            CHECK(athread_create(&tid[i], NULL, thread_2, NULL));
            printf("created thread %d\n", i);
        }
        
        for(int i=0; i<3; i++){
            CHECK(athread_join(tid[i], NULL));
            printf("thread joined  %d\n", i);
        }

        fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n");
        
    }

    fprintf(stdout, "\n\033[1;34mcase 5 : joining on thread and collecting the return value \033[0m\n\n");
    {
        athread_t tid;
        CHECK(athread_create(&tid, NULL, thread_2, NULL));
        void * return_value;
        CHECK(athread_join(tid, &return_value));
        
        printf("expected return value  - 200\n");
        printf("collected return value - %d\n", *(int*)return_value);
        
        if(*(int*)return_value == 200){
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n");    
        }
        else{
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }
    }

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    THREAD EXIT\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 : thread return using return keyword\033[0m\n");
    {
        athread_t tid;

        CHECK(athread_create(&tid, NULL, ret_thread, NULL));

        void * return_value;
        
        CHECK(athread_join(tid, &return_value));
        

        printf("expected return value  - 500\n");
        printf("collected return value - %d\n", *(int*)return_value);
    
        if (*(int*)return_value == 500) {
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n"); 
        } else {
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }

    }

    fprintf(stdout, "\n\033[1;34mcase 2 : thread return using athread_exit()\033[0m\n");
    {
        athread_t tid;

        CHECK(athread_create(&tid, NULL, exit_thread, NULL));

        void * return_value;
        
        CHECK(athread_join(tid, &return_value));

        printf("expected return value  - 700\n");
        printf("collected return value - %d\n", *(int*)return_value);

        if (*(int*)return_value == 700) {
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n"); 
        } else {
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }

    }

    fprintf(stdout, "\n\033[1;34mcase 3 : thread return from sub function\033[0m\n");
    {
        athread_t tid;

        CHECK(athread_create(&tid, NULL, exit_thread_sub, NULL));

        void * return_value;
        
        CHECK(athread_join(tid, &return_value));

        printf("expected return value  - 800\n");
        printf("collected return value - %d\n", *(int*)return_value);

        if (*(int*)return_value == 800) {
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n"); 
        } else {
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }

    }

    // fprintf(stdout, "=============================\n");
    // fprintf(stdout, "    THREAD KILL\n");
    // fprintf(stdout, "=============================\n");

    // fprintf(stdout, "\n\033[1;34mcase 1 : Thread directed signal dispositions \033[0m\n");
    // {
    //     athread_t tid;
    //     CHECK(athread_create(&tid, NULL, signal_thread, NULL ));
    //     printf("created an ifnintely looping thread\n");
    //     printf("sending SIGINT signal\n");
    //     athread_kill(tid, SIGINT);
    // }


    // fprintf(stdout, "\n\033[1;34mcase 2 : process directed signal dispositions \033[0m\n");
    // {
    //     athread_t tid;
    //     CHECK(athread_create(&tid, NULL, signal_thread, NULL ));
    //     printf("created an ifnintely looping thread\n");
    //     printf("sending SIGKILL signal\n");
    //     athread_kill(tid, SIGKILL);
    // }
    
    return 0;
}