#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
 // Get arguments
    if (argc != 4) {
        fprintf(stderr, "usage: %s nrows ncols filename\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    long long nrows = atoll(argv[1]);
    long long ncols = atoll(argv[2]);
    const char *path = argv[3];
 // Open file for writing
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
 // Set seed for random number generator
    srand48(time(0));
 // Write random numbers
    long long length = nrows * ncols;
    for (long long i = 0; i < length; i++) {
        double random_number = drand48();
        if (fwrite(&random_number, sizeof (double), 1, file) != 1) {
            fprintf(stderr, "write failed\n");
            exit(EXIT_FAILURE);
        }
    }
 // Close file
    fclose(file);
}
