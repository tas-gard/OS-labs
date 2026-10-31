#include "general.h"
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char filename[BUFFER_SIZE];
    int pos = 0;
    char c;

    write(STDOUT_FILENO, "Enter filename: ", 16);
    while (pos < BUFFER_SIZE - 1) {
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n == -1) {
            perror("read");
            return 1;
        }
        if (n == 0 || c == '\n') break;
        filename[pos++] = c;
    }
    filename[pos] = '\0';

    if (pos == 0) {
        fprintf(stderr, "Empty filename\n");
        return 1;
    }

    char shm_name[64], empty_name[64], full_name[64];
    long pid_number = (long)getpid();
    snprintf(shm_name, sizeof(shm_name), "/shm_%ld", pid_number);
    snprintf(empty_name, sizeof(empty_name), "/empty_%ld", pid_number);
    snprintf(full_name, sizeof(full_name), "/full_%ld", pid_number);

    int shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    if (ftruncate(shm_fd, sizeof(GeneralData)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }

    GeneralData *general = mmap(NULL, sizeof(GeneralData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    if (general == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }
    general->size = 0;
    general->error = 0;

    sem_t *empty = sem_open(empty_name, O_CREAT | O_EXCL, 0600, 0);
    if (empty == SEM_FAILED) {
        perror("sem_open empty");
        munmap(general, sizeof(GeneralData));
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }
    sem_t *full = sem_open(full_name, O_CREAT | O_EXCL, 0600, 0);
    if (full == SEM_FAILED) {
        perror("sem_open full");
        sem_close(empty);
        sem_unlink(empty_name);
        munmap(general, sizeof(GeneralData));
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        sem_close(empty);
        sem_close(full);
        sem_unlink(empty_name);
        sem_unlink(full_name);
        munmap(general, sizeof(GeneralData));
        close(shm_fd);
        shm_unlink(shm_name);
        return 1;
    }

    if (pid == 0) {
        execl("./child.out", "child.out", filename,
              shm_name, empty_name, full_name, (char *)NULL);
        perror("execl");
        general->error = 1;
        sem_post(empty);
        _exit(1);
    }

    int error = 0;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        if (sem_wait(empty) == -1) {
            perror("sem_wait");
            error = 1;
            break;
        }
        if (general->error) {
            fprintf(stderr, "Child process error\n");
            error = 1;
            break;
        }
        memcpy(general->buffer, buffer, (size_t)bytes_read);
        general->size = (int)bytes_read;
        if (sem_post(full) == -1) {
            perror("sem_post");
            error = 1;
            break;
        }
    }

    if (bytes_read == -1) {
        perror("read");
        error = 1;
    }

    if (!error) {
        if (sem_wait(empty) == -1) {
            perror("sem_wait");
            error = 1;
        } else if (general->error) {
            fprintf(stderr, "Child process error\n");
            error = 1;
        } else {
            general->size = 0;  
            if (sem_post(full) == -1) {
                perror("sem_post");
                error = 1;
            }
        }
    }

    if (error) kill(pid, SIGTERM);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        error = 1;
    } else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        error = 1;
    }

    sem_close(empty);
    sem_close(full);
    sem_unlink(empty_name);
    sem_unlink(full_name);
    munmap(general, sizeof(GeneralData));
    close(shm_fd);
    shm_unlink(shm_name);

    return error ? 1 : 0;
}