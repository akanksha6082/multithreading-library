#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include "athread.h"


#define THREAD_COUNT 5

#define CHECK(CALL)                                                             \
    {                                                                           \
        int result;                                                             \
        if ((result = (CALL)) != 0) {                                           \
            fprintf(stderr, "\nERROR: %s (%s)\n\n", strerror(result), #CALL);   \
        }                                                                       \
    }    

int row_1 = 0, col_1 = 0, row_2 = 0, col_2 = 0;

struct gmatrix{
    long long int ** matrix_1;
    long long int ** matrix_2;
    long long int ** result;
};

struct gmatrix gmatrix;


/*Matrix Input*/
void scan_matrix(int flag, int row, int col, FILE * fp){
    long long int ** matrix = (long long int **)malloc( sizeof(long long int*) * row);
    for(int i=0; i < row; i++){
        (matrix[i]) = (long long int *) malloc(sizeof(long long int) * col);
    }

    printf("%d %d\n", row, col);
    for(int i=0; i<row; i++){
        for(int j=0; j< col; j++){
            fscanf(fp, "%lld", &matrix[i][j]);
            fprintf(stdout, "%-5lld", matrix[i][j]);
        }
        printf("\n");
            
            
    }

    if(flag == 1)
        gmatrix.matrix_1 = matrix;
    else    
        gmatrix.matrix_2 = matrix;
}

/*Print Resultant Matrix*/
void display_result(int row, int col){

    printf("%d %d\n", row, col);

    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            printf("%-5lld", gmatrix.result[i][j]);
        }
        printf("\n");
    }
}

/*Thread function for Matrix Multiplication*/
void * multiply(void * args){
    
    long long int sum = 0;
    int * id = (int *)args;
    for(int i = *id; i < row_1; i += THREAD_COUNT){
        for(int j = 0; j< col_2; j++){
            sum = 0;
            for(int k = 0; k < row_2; k++){
                sum += gmatrix.matrix_1[i][k] * gmatrix.matrix_2[k][j];  
            }
            gmatrix.result[i][j] = sum;
        }
    }
    athread_exit(NULL);
}


int main( int argc, char ** argv){

    chdir("test/data");

    FILE * fp = fopen(argv[1], "r");
    
    
    if(fp == NULL){
        fprintf(stdout, "error: opening file\n");
        exit(1);
    }

    fscanf(fp, "%d", & row_1);
    fscanf(fp, "%d", &col_1);


    if(row_1 < 1 || col_1 < 1){
        exit(1);
    }

    printf("Matrix 1\n");
    scan_matrix( 1, row_1, col_1, fp);
    printf("\n");

    fscanf(fp, "%d", &row_2);
    fscanf(fp, "%d", &col_2);
  

    if(row_2 < 1 || col_2 < 1 || col_1 != row_2){
        free(gmatrix.matrix_1);
        exit(1);
    } 

    printf("Matrix 2\n");
    scan_matrix(2, row_2, col_2, fp);
    printf("\n");
   
    /*Resultant Matrix*/
    gmatrix.result = ( long long int** )malloc(sizeof(long long int*) * row_1);
    for(int i=0;i < row_1; i++){
        gmatrix.result[i] = (long long int*)malloc(sizeof(long long int) * col_2);
        for(int j = 0; j < col_2; j++){
            gmatrix.result[i][j] = 0;
        }
    }

    /*Thread Identifier*/
    athread_t tid[THREAD_COUNT]; 

    
    /*Set Thread attributes*/
    athread_attr_t attr;    

    
    /*Initialise the attributes to default values*/                
    CHECK(athread_attr_init(&attr));            

    int array[THREAD_COUNT];
    for(int i=0; i<THREAD_COUNT; i++)
        array[i] = i;


    /*create threads*/
    for(int i=0; i< THREAD_COUNT; i++){
        CHECK(athread_create(&tid[i], &attr, multiply, &array[i]));
    }

    /*wait for threads to finish*/
    for(int i =0; i<THREAD_COUNT; i++){
        CHECK(athread_join(tid[i], NULL));
    }

    printf("Resultant Matrix\n");
    display_result(row_1, col_2);

    free(gmatrix.matrix_1);
    free(gmatrix.matrix_2);
    free(gmatrix.result);

    return 0;
}
