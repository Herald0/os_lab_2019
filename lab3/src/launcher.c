#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        execl("./sequential_min_max", "sequential_min_max", "3", "3", NULL);
        perror("execl failed");
        return 1;
    } else {
        int status;
        waitpid(pid, &status, 0); // Ждем завершения дочернего процесса
        printf("sequential_min_max завершился с кодом %d\n", WEXITSTATUS(status));
    }

    return 0;
}
