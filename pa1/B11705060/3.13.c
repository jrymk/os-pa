#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: a.out <non-negative integer>\n");
        return -1;
    }

    int seq = atoi(argv[1]);

    if (seq < 0) {
        fprintf(stderr, "an integer >= 0 is required\n");
        return -1;
    }

    pid_t pid;
    pid = fork();

    if (pid < 0) // error occured
    {
        fprintf(stderr, "fork failed\n");
        return -1;
    } else if (pid == 0) // child process
    {
        unsigned long long v0 = 0, v1 = 1;

        for (int i = 0; i < seq && i < 2; i++)
            fprintf(stdout, (i == 0) ? "%d" : " %d", i); // fib_0 = 0, fib_1 = 1

        for (int i = 2; i < seq; i++) {
            unsigned long long v2 = v0 + v1;
            v0 = v1;
            v1 = v2;
            fprintf(stdout, " %lld", v2);
        }

        fprintf(stdout, "\n");
    } else // parent process
    {
        wait(NULL);
        // child complete
    }
    return 0;
}