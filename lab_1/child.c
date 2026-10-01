#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BLOCK_SIZE 1024
#define BUFFER_SIZE 2048

int main(int argc, char *argv[]){
    if (argc != 2) {
        write(STDERR_FILENO, "wrong arguments\n", 16);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    int file_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (file_fd == -1) {
        write(STDERR_FILENO, "open error\n", 11);
        return EXIT_FAILURE;
    }

    char block[BLOCK_SIZE];
    char ring[BUFFER_SIZE];
    char number[BLOCK_SIZE + 1];

    size_t number_len = 0;
    size_t head = 0;
    size_t tail = 0;
    size_t count = 0;
    ssize_t bytes_read;
    float sum = 0.0f;

    while ((bytes_read = read(STDIN_FILENO, block, BLOCK_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (count == BUFFER_SIZE) {
                write(STDERR_FILENO, "buffer overflow\n", 16);
                close(file_fd);
                return EXIT_FAILURE;
            }

            ring[tail] = block[i];
            tail = (tail + 1) % BUFFER_SIZE;
            ++count;
        }

        while (count > 0) {
            char c = ring[head];
            head = (head + 1) % BUFFER_SIZE;
            --count;

            if (c == ' ' || c == '\t' || c == '\n') {
                if (number_len > 0) {
                    number[number_len] = '\0';
                    char *end;
                    float value = strtof(number, &end);

                    if (end == number || *end != '\0') {
                        write(STDERR_FILENO, "invalid number\n", 15);
                        close(file_fd);
                        return EXIT_FAILURE;
                    }

                    sum += value;
                    number_len = 0;
                }

                if (c == '\n') {                    
                    char output[64];
                    int len = snprintf(output, sizeof(output), "%f\n", sum);
                    if (len < 0) {
                        close(file_fd);
                        return EXIT_FAILURE;
                    }
                
                    if (write(file_fd, output, (size_t)(len)) == -1) {
                        write(STDERR_FILENO, "write error\n", 12);
                        close(file_fd);
                        return EXIT_FAILURE;
                    }

                    sum = 0.0f;
                }
            }
            else {
                if (number_len == BLOCK_SIZE) {
                    write(STDERR_FILENO, "number too long\n", 16);
                    close(file_fd);
                    return EXIT_FAILURE;
                }

                number[number_len++] = c;
            }
        }
    }

    if (bytes_read == -1) {
        write(STDERR_FILENO, "read error\n", 11);
        close(file_fd);
        return EXIT_FAILURE;
    }
    
    if (number_len > 0) {
        number[number_len] = '\0';

        char *end;
        float value = strtof(number, &end);

        if (end == number || *end != '\0') {
            write(STDERR_FILENO, "invalid number\n", 15);
            close(file_fd);
            return EXIT_FAILURE;
        }

        sum += value;

        char output[64];
        int len = snprintf(output, sizeof(output), "%f\n", sum);

        if (len < 0) {
            close(file_fd);
            return EXIT_FAILURE;
        }

        if (write(file_fd, output, (size_t)len) == -1) {
            write(STDERR_FILENO, "write error\n", 12);
            close(file_fd);
            return EXIT_FAILURE;
        }
    }

    close(file_fd);
    return EXIT_SUCCESS;
}