#include "sort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <max_threads>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    max_threads = atoi(argv[2]);

    int *a = malloc((size_t)n * sizeof(int));
    int *b = malloc((size_t)n * sizeof(int));
    if (a == NULL || b == NULL) {
        printf("Memory allocation error\n");
        free(a);
        free(b);
        return 1;
    }

    srand(42);
    for (int i = 0; i < n; i++)
        a[i] = rand();
    memcpy(b, a, (size_t)n * sizeof(int));

    double start = get_time();
    quick_sort(a, 0, n - 1);
    double time = get_time() - start;

    printf("PID: %ld\n", (long)getpid());
    printf("Array size: %d, thread limit: %d\n", n, max_threads);
    fflush(stdout);

    start = get_time();
    parallel_sort(b, 0, n - 1);
    double parallel_time = get_time() - start;

    if (memcmp(a, b, (size_t)n * sizeof(int)) != 0) {
        printf("Sorting error\n");
        free(a);
        free(b);
        return 1;
    }

    printf("Sequential: %.3f ms\n", time);
    printf("Parallel:   %.3f ms\n", parallel_time);
    printf("Speedup:    %.3f\n", time / parallel_time);
    printf("Efficiency: %.3f\n", time / parallel_time / max_threads);
    printf("Peak threads: %d\n", peak_threads);
    printf("Sort: OK\n");

    free(a);
    free(b);
    return 0;
}
