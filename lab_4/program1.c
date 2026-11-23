#include "functions.h"
#include <stdio.h>

int main(void) {
    int command;
    float a, b, e;
    int x, y;

    printf("1 a b e - integral, 2 a b - primes\n");

    while (scanf("%d", &command) == 1) {
        if (command == 1) {
            if (scanf("%f %f %f", &a, &b, &e) != 3) break;
            printf("Integral: %f\n", sin_integral(a, b, e));
        } else if (command == 2) {
            if (scanf("%d %d", &x, &y) != 2) break;
            printf("Primes: %d\n", prime_count(x, y));
        }
    }
    return 0;
}