//C code that finds the missing 2 numbers in 100 million non-consecutive 64-bit numbers


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_COUNT 100000000
#define FILENAME "random_numbers2.bin"

int main() {
    FILE *fp;
    uint32_t *numbers;
    uint8_t *presence;
    uint32_t num;

    // Allocate memory for the array to track presence of numbers
    presence = (uint8_t *)calloc(NUM_COUNT, sizeof(uint8_t));
    if (presence == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    // Open the binary file for reading
    fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file.\n");
        free(presence);
        return EXIT_FAILURE;
    }

    // Read numbers from the file and mark their presence in the array
    while (fread(&num, sizeof(uint32_t), 1, fp) == 1) {
        if (num < NUM_COUNT) {
            presence[num] = 1;
        }
    }

    // Close the file
    fclose(fp);

    // Search for the missing numbers
    int missing_count = 0;
    for (uint32_t i = 0; i < NUM_COUNT; i++) {
        if (presence[i] == 0) {
            printf("Missing number: %u\n", i);
            missing_count++;
            if (missing_count == 2) {
                break; // Found both missing numbers, exit loop
            }
        }
    }

    // Free allocated memory
    free(presence);

    return EXIT_SUCCESS;
}

