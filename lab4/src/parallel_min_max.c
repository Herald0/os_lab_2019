#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int pnum = -1;
pid_t *child_pids = NULL;

void handle_alarm(int sig) {
    printf("Timeout reached. Terminating child processes...\n");
    for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL);
        }
    }
    exit(1);
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    int timeout = -1;
    bool with_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"timeout", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        break;
                    case 3:
                        timeout = atoi(optarg);
                        break;
                    case 4:
                        with_files = true;
                        break;
                    
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case 'f':
                with_files = true;
                break;

            case '?':
                break;

            default:
                printf("getopt returned character code 0%o\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    if (timeout > 0) {
        signal(SIGALRM, handle_alarm);
        alarm(timeout);
    }

    // while (1) {
        
    // }

    child_pids = malloc(sizeof(pid_t) * pnum);
    for (int i = 0; i < pnum; i++) {
        pid_t child_pid = fork();
        if (child_pid >= 0) {
            child_pids[i] = child_pid;
            active_child_processes += 1;
            if (child_pid == 0) 
            {
                int chunk_size = array_size / pnum;
                int start_index = i * chunk_size;
                int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

                int min = INT_MAX;
                int max = INT_MIN;

                for (int j = start_index; j < end_index; j++) {
                    if (array[j] < min) min = array[j];
                    if (array[j] > max) max = array[j];
                }

                if (with_files) {
                    char filename[25];
                    snprintf(filename, sizeof(filename), "minmax_%d.txt", i);
                    FILE *file = fopen(filename, "w");
                    if (file != NULL) {
                        fprintf(file, "%d %dn", min, max);
                        fclose(file);
                    } else {
                        perror("Error opening file");
                    }
                } else {
                    if (write(STDOUT_FILENO, &min, sizeof(min)) == -1) {
                        perror("Error writing min");
                    }
                    if (write(STDOUT_FILENO, &max, sizeof(max)) == -1) {
                        perror("Error writing max");
                    }
                }
                exit(0);
            }
        } 
        else {
            perror("Fork failed");
            return 1;
        }
    }

    while (active_child_processes > 0) {
        wait(NULL);
        active_child_processes -= 1;
    }

    if (timeout > 0) {
        alarm(0);
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) {
        int min = INT_MAX; 
        int max = INT_MIN; 

        if (with_files) { 
            char filename[25]; 
            snprintf(filename, sizeof(filename), "minmax_%d.txt", i); 
            FILE *file = fopen(filename, "r"); 
            if (file != NULL) { 
                if (fscanf(file, "%d %d", &min, &max) != 2) { 
                    perror("Error reading from file"); 
                } 
                fclose(file); 
            }  
            else { 
                perror("Error opening file"); 
            } 
        } 
        else { 
            if (read(STDIN_FILENO, &min, sizeof(min)) == -1) { 
                perror("Error reading min"); 
            } 
            if (read(STDIN_FILENO, &max, sizeof(max)) == -1) { 
                perror("Error reading max"); 
            } 
        } 

        if (min < min_max.min) min_max.min = min; 
        if (max > min_max.max) min_max.max = max; 
    } 

    struct timeval finish_time; 
    gettimeofday(&finish_time, NULL); 

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0; 
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0; 

    free(array); 
    free(child_pids);

    printf("Min: %d\n", min_max.min); 
    printf("Max: %d\n", min_max.max); 
    printf("Time: %fms\n", elapsed_time); 

    return 0; 
}