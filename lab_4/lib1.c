#include "functions.h"
#include <math.h>

float sin_integral(float a, float b, float e) {
    if (e <= 0) return 0;

    float sum = 0;
    for (float x = a; x < b; ) {
        float h = e;
        if (x + h > b) h = b - x;

        sum += sinf(x) * h;
        x += h;
    }
    return sum;
}

int prime_count(int a, int b) {
    int count = 0;

    for (int i = a; i <= b; i++) {
        if (i < 2) continue;

        int prime = 1;
        for (int j = 2; j < i; j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }
        if (prime) count++;
    }
    return count;
}