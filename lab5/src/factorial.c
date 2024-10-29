#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

typedef struct {
    int start;
    int end;
    long long *result;
    pthread_mutex_t *mutex;
    int mod;
} ThreadData;

void* factorial_worker(void* arg) {
    ThreadData *data = (ThreadData*)arg;
    long long partial_result = 1;

    for (int i = data->start; i <= data->end; i++) {
        partial_result = (partial_result * i) % data->mod;
    }

    pthread_mutex_lock(data->mutex);
    *data->result = (*data->result * partial_result) % data->mod;
    pthread_mutex_unlock(data->mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int k = -1, pnum = 1, mod = 1;

    int option;
    while ((option = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (option) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> -p <threads> -m <mod>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (k < 0 || pnum < 1 || mod < 1) {
        fprintf(stderr, "некорректные аргументы.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[pnum];
    ThreadData thread_data[pnum];
    long long result = 1;
    pthread_mutex_t mutex;

    pthread_mutex_init(&mutex, NULL);

    int range = k / pnum;
    for (int i = 0; i < pnum; i++) {
        thread_data[i].start = i * range + 1;
        thread_data[i].end = (i == pnum - 1) ? k : (i + 1) * range;
        thread_data[i].result = &result;
        thread_data[i].mutex = &mutex;
        thread_data[i].mod = mod;

        pthread_create(&threads[i], NULL, factorial_worker, &thread_data[i]);
    }

    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Факториал %d по модулю %d = %lld\n", k, mod, result);

    pthread_mutex_destroy(&mutex);

    return 0;
}
