#include "sort.h"
#include <stdatomic.h>

#define MIN_PART 10000

typedef struct {
    int *array;
    int left;
    int right;
} Task;

static atomic_flag mutex = ATOMIC_FLAG_INIT;

void mutex_lock(void) {
    while (atomic_flag_test_and_set(&mutex)) {
        // Ждем
    }
}

void mutex_unlock(void) {
    atomic_flag_clear(&mutex);
}

int max_threads;
int active_threads = 1;
int peak_threads = 1;

static int partition(int *a, int left, int right) {
    int pivot = a[(left + right) / 2];
    int i = left - 1;
    int j = right + 1;

    while (1) {
        do { i++; } while (a[i] < pivot);
        do { j--; } while (a[j] > pivot);

        if (i >= j) return j;

        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

void quick_sort(int *a, int left, int right) {
    if (left >= right) return;

    int p = partition(a, left, right);
    quick_sort(a, left, p);
    quick_sort(a, p + 1, right);
}

static void *thread_sort(void *arg) {
    Task *task = (Task *)arg;
    parallel_sort(task->array, task->left, task->right);
    return NULL;
}

void parallel_sort(int *a, int left, int right) {
    if (left >= right) return;

    int p = partition(a, left, right);
    int create = 0;

    if (max_threads > 1 && right - left >= MIN_PART) {
        mutex_lock();
        if (active_threads < max_threads) {
            active_threads++;
            create = 1;
        }
        mutex_unlock();
    }

    if (create) {
        pthread_t thread;
        Task task = {a, left, p};

        if (pthread_create(&thread, NULL, thread_sort, &task) == 0) {
            mutex_lock();
            if (active_threads > peak_threads)
                peak_threads = active_threads;
            mutex_unlock();

            parallel_sort(a, p + 1, right);
            pthread_join(thread, NULL);

            mutex_lock();
            active_threads--;
            mutex_unlock();
            return;
        }

        mutex_lock();
        active_threads--;
        mutex_unlock();
    }

    parallel_sort(a, left, p);
    parallel_sort(a, p + 1, right);
}

double get_time(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0;
}