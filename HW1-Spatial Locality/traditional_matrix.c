#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char* argv[]) {
    struct timeval start, end;
    int rows, cols, i, j, k;

    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    rows = cols = atoi(argv[1]);

    int **a, **b, **c;

    // Allocate memory for matrices
    a = (int **)malloc(rows * sizeof(int *));
    b = (int **)malloc(rows * sizeof(int *));
    c = (int **)malloc(rows * sizeof(int *));
    for (i = 0; i < rows; ++i) {
        a[i] = (int *)malloc(cols * sizeof(int));
        b[i] = (int *)malloc(cols * sizeof(int));
        c[i] = (int *)malloc(cols * sizeof(int));
    }

    // Initialize matrices a and b (c is initialized to 0)
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            a[i][j] = rand(); // You can initialize with any values you want
            b[i][j] = rand();
            c[i][j] = 0;
        }
    }

    gettimeofday(&start, NULL); // Start timing

    // Perform traditional matrix multiplication
    for (i = 0; i < rows; ++i) {
        for (j = 0; j < cols; ++j) {
            for (k = 0; k < cols; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }

    gettimeofday(&end, NULL); // End timing

    // Calculate and print execution time
    double exec_time = (double)(end.tv_sec - start.tv_sec) +
                       (double)(end.tv_usec - start.tv_usec) / 1.0e6;
    printf("Traditional matrix multiplication completed in %.6f seconds.\n", exec_time);

    // Free allocated memory
    for (i = 0; i < rows; ++i) {
        free(a[i]);
        free(b[i]);
        free(c[i]);
    }
    free(a);
    free(b);
    free(c);

    return 0;
}
