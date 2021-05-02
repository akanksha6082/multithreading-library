/*
 * validates the standarad interface provided by library
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "../include/athread.h"

#define CHECK(CALL)                                                             \
    {                                                                           \
        int result;                                                             \
        if ((result = (CALL)) != 0) {                                           \
            fprintf(stderr, "\nERROR: %s (%s)\n\n", strerror(result), #CALL);   \
        }                                                                       \
    }    

#define CHECK_PASS(CALL)                                                        \
    {                                                                           \
        int result;                                                             \
        if ((result = (CALL)) == 0) {                                           \
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n");                \
        }                                                                       \
        else{                                                                   \
            fprintf(stderr, "\nERROR: %s (%s)\n\n", strerror(result), #CALL);   \
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");                \
        }                                                                       \
                                                                                \
    }                                                                           \

#define CHECK_FAIL(CALL)                                                        \
    {                                                                           \
        int result;                                                             \
        if ((result = (CALL)) != 0) {                                           \
            fprintf(stderr, "\nERROR: %s (%s)\n\n", strerror(result), #CALL);   \
            fprintf(stdout, "\033[0;32mTEST PASS\033[0m\n\n");                  \
        }                                                                       \
        else{                                                                   \
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");                \
        }                                                                       \
    }                                                                           \

static int run = 1;

void signal_handler(int signum){
    fprintf(stdout, "thread recieved %s\n", strsignal(signum));
    run = 0;
}

void * thread_1(void * args){
    int a = 100;
    void * ptr = &a;
    return ptr;
}

void * thread_2(void* args){
    int a = 200;
    void * ptr = &a;
    return ptr;
}

/*thread returns using return keyword*/
void * ret_thread(void* args){
    int a = 500;
    void * ptr = &a;
    printf("Thread return using return keyword\n\n");
    return ptr;
}

/*thread returns using athread_exit() */
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

/*infinitely running thread*/
void * signal_thread(void * args){
    int a = 1500;
    void * p = &a;
    while(run);
    athread_exit(p);
    
}

void * thread_4(void * args){
    int a = 1000;
    void * p = &a;
    sleep(1);
    athread_exit(p);
}

void * thread_3(void * args){
    void * ret;
    athread_t tid;
    CHECK(athread_create(&tid, NULL, thread_4, NULL));
    fprintf(stdout, "created thread within thread (sub-thread)\n");
    CHECK(athread_join(tid, &ret));
    sleep(1);
    athread_exit(ret);
}

int main(int argc, char ** argv){
    
    athread_init();

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    ATTRIBUTE HANDLING\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 : handling detach state of thread\033[0m\n");

    {
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
        int state;
        CHECK(athread_attr_init(&attr));
        CHECK(athread_attr_setdetachstate(&attr, ATHREAD_CREATE_DETACHED));
        CHECK(athread_attr_getdetachstate(&attr, &state));
        CHECK(athread_create(&tid, &attr, thread_1, NULL));
        if(state == 0){
            fprintf(stdout, "created detached thread\n");  
        }
        else{
            fprintf(stdout, "failed to created detached thread\n");
        }
        
        fprintf(stdout, "Joining on detached thread\n");
        CHECK_FAIL(athread_join(tid, NULL ));
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

    fprintf(stdout, "\n\033[1;34mcase 3 : thread join on itself\033[0m\n");
    {
        athread_t tid;
        CHECK(athread_create(&tid, NULL, thread_1, NULL));
        CHECK_FAIL(athread_join(athread_self(), NULL)); 
    }

    fprintf(stdout, "\n\033[1;34mcase 4 : thread join on detached thread\033[0m\n");
    {
        athread_t tid;
        athread_attr_t attr;
        CHECK(athread_attr_init(&attr));
        CHECK(athread_attr_setdetachstate(&attr, ATHREAD_CREATE_DETACHED));
        CHECK(athread_create(&tid, &attr, thread_1, NULL));
        CHECK_FAIL(athread_join(tid, NULL));

    }

    fprintf(stdout, "\n\033[1;34mcase 5 : thread join on multiple thread\033[0m\n\n");
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

    fprintf(stdout, "\n\033[1;34mcase 6 : joining on thread and collecting the return value \033[0m\n\n");
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
    
    fprintf(stdout, "\n\033[1;34mcase 7 : joining on thread which is joining on some other thread\033[0m\n\n");
    {
        athread_t tid;
        int expected_ret_val = 1000;
        void * ret;
        CHECK(athread_create(&tid, NULL, thread_3, NULL));
        fprintf(stdout, "created thread\n");
        CHECK(athread_join(tid, &ret));

        fprintf(stdout, "expected return value sub thread in main = %d\n", expected_ret_val);
        fprintf(stdout, "collected return value from sub thread in main = %d\n", *(int*)ret);

        if(expected_ret_val == *(int*)ret){
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

    fprintf(stdout, "=============================\n");
    fprintf(stdout, "    THREAD KILL\n");
    fprintf(stdout, "=============================\n");

    fprintf(stdout, "\n\033[1;34mcase 1 :  Testing SIGINT signal \033[0m\n");
    {
        athread_t tid;
        void * ret;

        signal(SIGINT, signal_handler);

        CHECK(athread_create(&tid, NULL, signal_thread, NULL ));
        fprintf(stdout, "created an infintely looping thread\n");
        fprintf(stdout, "sending SIGINT signal\n");
        
        CHECK(athread_kill(tid, SIGINT));
        CHECK(athread_join(tid, &ret));

        fprintf(stdout, "expected return value after thread recieves signal = %d\n", 1500);
        fprintf(stdout, "collected return value after thread recieves signal = %d\n", *(int *)ret);

        if(*(int*)ret == 1500){
            fprintf(stdout, "\n\033[0;32mTEST PASS\033[0m\n\n"); 
        }
        else{
            fprintf(stdout, "\n\033[0;31mTEST FAIL\033[0m\n\n");
        }
    }
    
    return 0;
}