#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: a.out <source file> <destination file>\n");
        return -1;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        fprintf(stderr, "pipe failed\n");
        return -1;
    }

    FILE *f_src = fopen(argv[1], "rb"); // read file in binary mode

    if (f_src == NULL) {
        fprintf(stderr, "open source file %s failed\n", argv[1]);
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t sz; // number of read bytes
    size_t bytes_read = 0;
    while (sz = fread(buffer, sizeof(buffer[0]), BUFFER_SIZE, f_src)) {
        write(pipe_fd[WRITE_END], buffer, sz);
        bytes_read += sz;
    }

    close(pipe_fd[WRITE_END]);

    pid_t pid;
    pid = fork();

    if (pid < 0) // error occured
    {
        fprintf(stderr, "fork failed\n");
        return -1;
    } else if (pid == 0) // child process
    {
        FILE *f_dest = fopen(argv[2], "wb"); // write file in binary mode (overwrite mode)
        unsigned char buffer[BUFFER_SIZE];
        size_t bytes_wrote = 0;
        size_t sz; // number of read bytes

        if (f_dest == NULL) {
            fprintf(stderr, "open/create destination file %s failed\n", argv[2]);
            return -1;
        }

        while (sz = read(pipe_fd[READ_END], buffer, BUFFER_SIZE)) {
            if (sz < 0) {
                fprintf(stderr, "pipe read error\n");
                return -1;
            }
            fwrite(buffer, sizeof(buffer[0]), sz, f_dest);
            bytes_wrote += sz;
        }
        if (bytes_read == bytes_wrote)
            fprintf(stdout, "wrote %ld bytes\n", bytes_wrote);
        else
            fprintf(stderr, "file copy error: read %ld bytes, wrote %ld bytes\n", bytes_read, bytes_wrote);
    } else // parent process
    {
        wait(NULL);
    }
    close(pipe_fd[READ_END]);
    return 0;
}