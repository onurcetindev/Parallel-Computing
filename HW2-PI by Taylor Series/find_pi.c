#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define TERMS 1000000  // Total number of terms in the Taylor series

struct ThreadArgs {
    int start;
    int end;
    double result;
};

void *computePi(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    double sum = 0.0;

    for (int i = threadArgs->start; i < threadArgs->end; i++) {
        if (i % 2 == 0) {
            sum += 1.0 / (2 * i + 1);  // Adding positive term
        } else {
            sum -= 1.0 / (2 * i + 1);  // Subtracting negative term
        }
    }

    threadArgs->result = sum;
    pthread_exit(NULL);
}

int main() {
    int numThreads, totalOps;
    printf("Enter the total number of operations (add/subtract): ");
    scanf("%d", &totalOps);
    printf("Enter the number of compute nodes (threads, 1-64): ");
    scanf("%d", &numThreads);

    struct ThreadArgs threadArgs[numThreads];
    pthread_t threads[numThreads];

    int opsPerThread = totalOps / numThreads;
    int remainingOps = totalOps % numThreads;
    int currentStart = 0;

    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].start = currentStart;
        threadArgs[i].end = currentStart + opsPerThread + (i < remainingOps ? 1 : 0);
        currentStart = threadArgs[i].end;

        pthread_create(&threads[i], NULL, computePi, (void *)&threadArgs[i]);
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    double totalPi = 0.0;
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        totalPi += threadArgs[i].result;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    totalPi *= 4.0;  // Multiply by 4 as per the formula

    double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    printf("Estimated value of Pi: %.10lf\n", totalPi);
    printf("Computation time: %.6lf seconds\n", elapsedTime);

    return 0;
}
