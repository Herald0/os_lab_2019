#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "sum.h"
#include "utils.h"


void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    int *result = malloc(sizeof(int));
    *result = Sum(sum_args);
    return result;
}

int main(int argc, char **argv) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
        return 1;
    }

    uint32_t threads_num = atoi(argv[2]);
    uint32_t seed = atoi(argv[4]);
    uint32_t array_size = atoi(argv[6]);

    pthread_t threads[threads_num];
    int *array = malloc(sizeof(int) * array_size);
    if (!array) {
        perror("Failed to allocate memory");
        return 1;
    }

    GenerateArray(array, array_size, seed);

    struct SumArgs args[threads_num];
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * (array_size / threads_num);
        args[i].end = (i + 1) * (array_size / threads_num);
        if (i == threads_num - 1) {
            args[i].end = array_size;
        }

        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            fprintf(stderr, "Error: pthread_create failed!\n");
            free(array);
            return 1;
        }
    }

    clock_t start_time = clock();

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int *sum_result;
        pthread_join(threads[i], (void **)&sum_result);
        total_sum += *sum_result;
        free(sum_result);
    }

    clock_t end_time = clock();

    free(array);

    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Total: %d\n", total_sum);
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
