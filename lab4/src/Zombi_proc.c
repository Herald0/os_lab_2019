#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Ошибка");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Дочерний процесс: PID = %d\n", getpid());
        exit(0);
    } else {
        printf("Родительский процесс: PID = %d, создал дочерний PID = %d\n", getpid(), pid);
        sleep(10);
        printf("Родительский процесс завершил выполнение.\n");
    }

    return 0;
}
