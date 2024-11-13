#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "arpa/inet.h"
#include <pthread.h>

#include "factorial.h"

void parse_server_file(const char *filename, char servers[][50], int *server_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open server file");
        exit(EXIT_FAILURE);
    }

    char line[50];
    *server_count = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        strcpy(servers[*server_count], line);
        (*server_count)++;
    }

    fclose(file);
}

void* func(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    char buffer[sizeof(uint64_t) * 3];
    memcpy(buffer, &fargs->begin, sizeof(uint64_t));
    memcpy(buffer + sizeof(uint64_t), &fargs->end, sizeof(uint64_t));
    memcpy(buffer + 2 * sizeof(uint64_t), &fargs->mod, sizeof(uint64_t));

    send(fargs->socket, buffer, sizeof(buffer), 0);

    uint64_t result;
    recv(fargs->socket, &result, sizeof(result), 0);
    printf("Result: %lu\n", result);

    close(fargs->socket);

    return (void *) result;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s k mod server_file\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint64_t k = strtoull(argv[1], NULL, 10);
    uint64_t mod = strtoull(argv[2], NULL, 10);

    char servers[10][50];
    int server_count;

    parse_server_file(argv[3], servers, &server_count);
    struct FactorialArgs args[server_count];
    pthread_t threads[server_count];
    uint64_t range_size = k / server_count;

    for (int i = 0; i < server_count; i++) {
        char *server_info = servers[i];

        char *ip_str = strtok(server_info, ":");
        char *port_str = strtok(NULL, ":");

        int port = atoi(port_str);

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Socket creation failed");
            continue;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, ip_str, &server_addr.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd);
            continue;
        }

        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sockfd);
            continue;
        }
        
        args[i].begin = i * range_size + 1;
        args[i].end = (i == server_count - 1) ? k : (i + 1) * range_size;
        args[i].mod = mod;
        args[i].socket = sockfd;

        if (pthread_create(&threads[i], NULL, func, (void *)&args[i])) {
            fprintf(stderr, "Error: pthread_create failed\n");
            close(sockfd);
            return EXIT_FAILURE;
        }
    }

    uint64_t total = 1;
    for (uint32_t i = 0; i < server_count; i++) {
        uint64_t result = 0;
        pthread_join(threads[i], (void **)&result);
        total = MultModulo(total, result, mod);
    }
    printf("Total: %lu\n", total);

    return EXIT_SUCCESS;
}
