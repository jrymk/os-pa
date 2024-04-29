#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SEQUENCE 10

typedef struct {
    long fib_sequence[MAX_SEQUENCE];
    int sequence_size;
} shared_data;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: a.out <non-negative integer less than %d>\n", MAX_SEQUENCE);
        return -1;
    }

    int cmdParam = atoi(argv[1]);

    if (cmdParam < 0) {
        fprintf(stderr, "an integer >= 0 is required\n");
        return -1;
    }

    if (cmdParam > MAX_SEQUENCE) {
        fprintf(stderr, "an integer <= MAX_SEQUENCE = %d is required\n", MAX_SEQUENCE);
        return -1;
    }

    int shm_fd = shm_open("fib", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_data));
    void *ptr = mmap(0, sizeof(shared_data), PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);

    ((shared_data *)ptr)->sequence_size = cmdParam;

    pid_t pid;
    pid = fork();

    if (pid < 0) // error occured
    {
        fprintf(stderr, "fork failed\n");
        return -1;
    } else if (pid == 0) // child process
    {
        long v0 = 0, v1 = 1;

        for (int i = 0; i < ((shared_data *)ptr)->sequence_size && i < 2; i++)
            ((shared_data *)ptr)->fib_sequence[i] = i; // fib_0 = 0, fib_1 = 1

        for (int i = 2; i < ((shared_data *)ptr)->sequence_size; i++) {
            long v2 = v0 + v1;
            v0 = v1;
            v1 = v2;
            ((shared_data *)ptr)->fib_sequence[i] = v2;
        }
    } else // parent process
    {
        wait(NULL);
        // child complete

        for (int i = 0; i < ((shared_data *)ptr)->sequence_size; i++)
            fprintf(stdout, i ? " %ld" : "%ld", ((shared_data *)ptr)->fib_sequence[i]);
        fprintf(stdout, "\n");
        shm_unlink("fib");
    }
    return 0;
}