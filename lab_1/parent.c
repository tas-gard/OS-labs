#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void) {
    char filename[BUFFER_SIZE];
    int pos = 0;
    char c;

    write(STDOUT_FILENO, "Enter filename: ", 16);

    while (pos < BUFFER_SIZE - 1){
        ssize_t n = read(STDIN_FILENO, &c, 1);

        if (n == -1){
            write(STDERR_FILENO, "read error\n", 11);
            return EXIT_FAILURE;
        }

        if (n == 0 || c == '\n'){
            break;
        }

        filename[pos++] = c;
    }

    filename[pos] = '\0';

    if (pos == 0){
        write(STDERR_FILENO, "empty filename\n", 15);
        return EXIT_FAILURE;
    }


    int pipe_fd[2];

    if (pipe(pipe_fd) == -1){
        write(STDERR_FILENO, "pipe error\n", 11);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        write(STDERR_FILENO, "fork error\n", 11);

        close(pipe_fd[0]);
        close(pipe_fd[1]);

        return EXIT_SUCCESS;
    }

if (pid == 0) {
    close(pipe_fd[1]);

    if (dup2(pipe_fd[0], STDIN_FILENO) == -1) {
        write(STDERR_FILENO, "dup2 error\n", 11);
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[0]);

    execl("./child.out", "child.out", filename, (char *)NULL);
    write(STDERR_FILENO, "exec error\n", 11);
    exit(EXIT_FAILURE);
} else {
    close(pipe_fd[0]);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0){
        ssize_t bytes_written = write(pipe_fd[1], buffer, bytes_read);

        if (bytes_written == -1){
            write(STDERR_FILENO, "write error\n", 12);
            close(pipe_fd[1]);
            wait(NULL);
            return EXIT_FAILURE;
        }
    }
    if (bytes_read == -1){
        write(STDERR_FILENO, "read error\n", 11);
        close(pipe_fd[1]);
        wait(NULL);
        return EXIT_FAILURE;
    }

    close(pipe_fd[1]);
    if (wait(NULL) == -1){
        write(STDERR_FILENO, "wait error\n", 11);
        return EXIT_FAILURE;
    }
}
}