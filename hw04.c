/*Name - Surname : Onur Çetin
  ID : 20200808050
  Course : Parallel Computing

  Implement a parallel Matrix multiplication code in C to compute the multiplication of
  two matrices of size 4096x4096 while considering nrow major ordering and multithreaded
  programming principles. You can use -Ofast optimization flag for better performance.

  gcc -pthread -Ofast hw04.c -o hw04
  ./hw04
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MATRIX_SIZE 4096
#define NUM_THREADS 4  

int matrix_a[MATRIX_SIZE][MATRIX_SIZE];
int matrix_b[MATRIX_SIZE][MATRIX_SIZE];
int result_matrix[MATRIX_SIZE][MATRIX_SIZE];


typedef struct {
    int start_row;
    int end_row;
} ThreadArgs;

// Function to perform matrix multiplication for a given range of rows
void* multiply(void* args) {
    ThreadArgs* thread_args = (ThreadArgs*)args;
    int start_row = thread_args->start_row;
    int end_row = thread_args->end_row;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            result_matrix[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                result_matrix[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

int main() {
    // Initialize matrices with random values
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            matrix_a[i][j] = rand() % 10;
            matrix_b[i][j] = rand() % 10;
        }
    }

    pthread_t threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    int rows_per_thread = MATRIX_SIZE / NUM_THREADS;
    
    // Create threads and assign rows for each thread
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].start_row = i * rows_per_thread;
        thread_args[i].end_row = (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, multiply, (void*)&thread_args[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

   //Test Case 
   printf("Result Matrix:\n");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", result_matrix[i][j]);
        }
        printf("\n");
    } 

    return 0;
}
