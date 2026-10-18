#ifndef SORT_H
#define SORT_H

#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <time.h>

extern int max_threads;
extern int peak_threads;

void quick_sort(int *a, int left, int right);
void parallel_sort(int *a, int left, int right);
double get_time(void);

#endif