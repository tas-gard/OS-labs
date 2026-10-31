#include "general.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static int add_number(char *number, int *len, float *sum) {
    if (*len == 0) return 0;
    number[*len] = '\0';
    char *end;
    float value = strtof(number, &end);
    if (end == number || *end != '\0') return -1;
    *sum += value;
    *len = 0;
    return 0;
}

static int write_sum(int fd, float sum) {
    char output[64];
    int len = snprintf(output, sizeof(output), "%f\n", sum);
    return write(fd, output, (size_t)len) == len ? 0 : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Write arguments\n");
        return 1;
    }

    int shm_fd = shm_open(argv[2], O_RDWR, 0);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    GeneralData *general = mmap(NULL, sizeof(GeneralData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (general == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }

    sem_t *empty = sem_open(argv[3], 0);
    sem_t *full = sem_open(argv[4], 0);
    if (empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        if (empty != SEM_FAILED) sem_close(empty);
        if (full != SEM_FAILED) sem_close(full);
        munmap(general, sizeof(GeneralData));
        close(shm_fd);
        return 1;
    }

    int file_fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1) {
        perror("open");
        general->error = 1;
        sem_post(empty);
        sem_close(empty);
        sem_close(full);
        munmap(general, sizeof(GeneralData));
        close(shm_fd);
        return 1;
    }

    sem_post(empty);

    char number[BUFFER_SIZE + 1];
    int number_len = 0;
    int line_started = 0;
    float sum = 0.0f;
    int error = 0;

    while (1) {
        if (general->size == 0) {
            sem_post(empty);
            break;
        }

        for (int i = 0; i < general->size; i++) {
            char c = general->buffer[i];
            if (c == ' ' || c == '\t' || c == '\n') {
                if (add_number(number, &number_len, &sum) == -1) {
                    fprintf(stderr, "Invalid number\n");
                    error = 1;
                    break;
                }
                if (c == '\n') {
                    if (write_sum(file_fd, sum) == -1) {
                        perror("write");
                        error = 1;
                        break;
                    }
                    sum = 0.0f;
                    line_started = 0;
                } else {
                    line_started = 1;
                }
            } else {
                if (number_len == BUFFER_SIZE) {
                    fprintf(stderr, "Number too long\n");
                    error = 1;
                    break;
                }
                number[number_len++] = c;
                line_started = 1;
            }
        }

        if (error) general->error = 1;
        if (sem_post(empty) == -1) {
            perror("sem_post");
            error = 1;
        }
        if (error) break;
    }

    if (!error && add_number(number, &number_len, &sum) == -1) {
        fprintf(stderr, "Invalid number\n");
        error = 1;
    }
    if (!error && line_started && write_sum(file_fd, sum) == -1) {
        perror("write");
        error = 1;
    }

    close(file_fd);
    sem_close(empty);
    sem_close(full);
    munmap(general, sizeof(GeneralData));
    close(shm_fd);
    return error ? 1 : 0;
}