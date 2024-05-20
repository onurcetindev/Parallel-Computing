// Onur Çetin 
// 20200808050
// Parallel Computing Matrix Normalization Homework

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

void generate_matrix(float *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = (float)rand() / RAND_MAX * 100.0;
    }
}

void print_matrix(float *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f\t", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int N;
    float *matrix = NULL;
    float *sub_matrix;
    float local_max = -FLT_MAX;
    float global_max;

    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return -1;
    }

    N = atoi(argv[1]);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0) {
            printf("Matrix size should be divisible by the number of processes.\n");
        }
        MPI_Finalize();
        return -1;
    }

    int rows_per_proc = N / size;
    sub_matrix = (float *)malloc(rows_per_proc * N * sizeof(float));

    if (rank == 0) {
        matrix = (float *)malloc(N * N * sizeof(float));
        generate_matrix(matrix, N, N);
        printf("Original matrix:\n");
        print_matrix(matrix, N, N);
    }

    MPI_Scatter(matrix, rows_per_proc * N, MPI_FLOAT, sub_matrix, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < rows_per_proc * N; i++) {
        if (sub_matrix[i] > local_max) {
            local_max = sub_matrix[i];
        }
    }

    MPI_Allreduce(&local_max, &global_max, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);

    for (int i = 0; i < rows_per_proc * N; i++) {
        sub_matrix[i] /= global_max;
    }

    float *normalized_matrix = NULL;
    if (rank == 0) {
        normalized_matrix = (float *)malloc(N * N * sizeof(float));
    }

    MPI_Gather(sub_matrix, rows_per_proc * N, MPI_FLOAT, normalized_matrix, rows_per_proc * N, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Normalized matrix:\n");
        print_matrix(normalized_matrix, N, N);
        free(matrix);
        free(normalized_matrix);
    }

    free(sub_matrix);
    MPI_Finalize();
    return 0;
}
