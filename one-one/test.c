#include <stdio.h>
#include "athread.h"
#include <string.h>
#include <unistd.h>

#define print(str) write(1, str, strlen(str))

long int c = 0, c1 =0, c2 = 0, run = 1;
int a;
void * f1(void *);
void * f2(void *);

athread_mutex_t mutex;

void * f1(void * args){
    scanf("%d", &a);
    printf("a = %d\n",a);
    while(run == 1){
        
        athread_mutex_lock(&mutex);
        c1++;
        c++;
        athread_mutex_unlock(&mutex);
       
    }
    return (void*)200;
}

void * f2(void * args){
    int a = 100;
    while(run == 1){
        
        athread_mutex_lock(&mutex);
        c2++;
        c++;
        athread_mutex_unlock(&mutex);
      
    }
    athread_exit((void*)a);
}

int main(){
    
    athread_init();

    athread_t t1, t2;

    athread_mutex_init(&mutex);

    void * retval;

    athread_create(&t1, NULL, f1, NULL);
    athread_create(&t2, NULL, f2, NULL);

    sleep(2);
    run = 0;

    athread_join(t1, NULL);
    athread_join(t2, &retval);
    
    printf("c = %ld  c1=%ld   c2 = %ld\n", c , c1, c2);
    printf("return value = %d\n", *((int*)retval));

    athread_mutex_destroy(&mutex);

    return 0;

}