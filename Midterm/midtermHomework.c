#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 5000
#define MASTER_RANK 0
#define FILENAME_A "A.bin"
#define FILENAME_B "B.bin"

void loadMatrixFromBinary(const char* filename, double* matrix, int rows, int cols) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s.\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    size_t elements_read = fread(matrix, sizeof(double), rows * cols, file);
    if (elements_read != rows * cols) {
        fprintf(stderr, "Error reading file %s. Expected %d elements, but read %zu elements.\n", filename, rows * cols, elements_read);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    fclose(file);
}

void matrixMultiplication(double* A, double* B, double* C, int rows, int cols, int local_rows) {
    for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < cols; j++) {
            double temp = 0.0; // Temporary variable to store the sum
            for (int k = 0; k < cols; k++) {
                temp += A[i * cols + k] * B[k * cols + j];
            }
            C[i * cols + j] = temp; // Assign the sum to the result matrix
        }
    }
}

void printMatrix(double* matrix, int rows, int cols) {   
     for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size < 2) {
        fprintf(stderr, "Program requires at least 2 MPI processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    double start_time, end_time;  // Variables to hold start and end time

    start_time = MPI_Wtime();  // Start measuring time

    // Load Matrices A and B from binary files
    double* A = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    double* B = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    double* C = (double*)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    loadMatrixFromBinary(FILENAME_A, A, MATRIX_SIZE, MATRIX_SIZE);
    loadMatrixFromBinary(FILENAME_B, B, MATRIX_SIZE, MATRIX_SIZE);

    MPI_Bcast(A, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);

    int rows_per_process = MATRIX_SIZE / size;
    int local_rows = (rank == size - 1) ? (MATRIX_SIZE - (size - 1) * rows_per_process) : rows_per_process;

    double* local_C = (double*)malloc(rows_per_process * MATRIX_SIZE * sizeof(double));
      if (local_C == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    matrixMultiplication(A + rank * rows_per_process * MATRIX_SIZE, B, local_C, rows_per_process, MATRIX_SIZE, local_rows);

    MPI_Gather(local_C, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, C, rows_per_process * MATRIX_SIZE, MPI_DOUBLE, MASTER_RANK, MPI_COMM_WORLD);

    end_time = MPI_Wtime();  // Stop measuring time
			     //
    if (rank == MASTER_RANK) {
        printf("Result matrix C:\n");
        printMatrix(C, MATRIX_SIZE, MATRIX_SIZE);
	printf("Time taken for matrix multiplication: %f seconds\n", end_time - start_time);
    }

    free(A);
    free(B);
    free(C);
    free(local_C);


    MPI_Finalize();

    return 0;
}
