#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        sleep(5);
        exit(5);
    }

    pid = fork();
    if (pid == 0) {
        sleep(1);
        exit(1);
    }

    wait(&status);
    wait(&status);
}