#ifndef GENERAL_H
#define GENERAL_H

#define _POSIX_C_SOURCE 200809L
#define BUFFER_SIZE 1024

typedef struct {
    int size;
    int error;
    char buffer[BUFFER_SIZE];
} GeneralData;

#endif