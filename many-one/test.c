#include<stdio.h>
#include "athread.h"
#define print(str) (write(1, str, strlen(str)))

long int c1=0, c2 =0, run =1;
void * f1(void * args){

    athread_t id = athread_self();
    printf("f1 = %d\n", id);
   
    while(c1 < 1000){
        c1 ++;
      
    }
  
    athread_exit(NULL);

}

void * f2(void * args){
    
    athread_t id = athread_self();
    printf("f2 = %d\n", id);
    while(c2 < 1000){
        c2 ++;
    }

    athread_exit(NULL);
}

void * f3(void * args){
    athread_t id = athread_self();
    printf("f3 = %d\n", id);
    int sum = 0;
    for(int i =0;i<10000; i++){
        for(int j =0; j<10000; j++){
            sum = i +j;
        }
    }
    printf("sum = %d\n", sum);
}

int main(int argc, char * argv[]){
    
    athread_init();
    athread_t t1, t2, t3;

    athread_create(&t3, f3, NULL);
    athread_create(&t1, f1, NULL);
    athread_create(&t2, f2, NULL);
    
 
    athread_join(t1, NULL);
    athread_join(t2, NULL);
    athread_join(t3, NULL);


    printf("c1 = %ld     c2 = %ld\n", c1, c2);

    return 0;

}
