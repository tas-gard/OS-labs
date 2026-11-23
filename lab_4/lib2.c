#include "functions.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

float sin_integral(float a, float b, float e) {
    if (e <= 0) return 0;

    float sum = 0;
    for (float x = a; x < b; ) {
        float h = e;
        if (x + h > b) h = b - x;

        sum += (sinf(x) + sinf(x + h)) * h / 2;
        x += h;
    }
    return sum;
}

int prime_count(int a, int b) {
    if (b < 2 || a > b) return 0;

    char *prime = malloc((size_t)b + 1);
    if (prime == NULL) return -1;
    memset(prime, 1, (size_t)b + 1);

    prime[0] = 0;
    prime[1] = 0;

    for (int i = 2; i * i <= b; i++) {
        if (prime[i]) {
            for (int j = i * i; j <= b; j += i)
                prime[j] = 0;
        }
    }

    int count = 0;
    for (int i = a; i <= b; i++) {
        if (i >= 2 && prime[i]) count++;
    }

    free(prime);
    return count;
}