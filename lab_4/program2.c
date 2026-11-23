#include <stdio.h>
#include <dlfcn.h>

int main(void) {
    const char *names[2] = {"./lib1.so", "./lib2.so"};
    int version = 0;
    int command;
    float a, b, e;
    int x, y;

    void *lib = dlopen(names[version], RTLD_LAZY);

    float (*integral)(float, float, float) = dlsym(lib, "sin_integral");
    int (*primes)(int, int) = dlsym(lib, "prime_count");

    printf("Implementation 1\n");
    printf("0 - switch, 1 a b e - integral, 2 a b - primes\n");

    while (scanf("%d", &command) == 1) {
        if (command == 0) {
            dlclose(lib);
            version = 1 - version;
            lib = dlopen(names[version], RTLD_LAZY);
            if (lib == NULL) {
                printf("Library error: %s\n", dlerror());
                return 1;
            }
            integral = dlsym(lib, "sin_integral");
            primes = dlsym(lib, "prime_count");
            printf("Implementation %d\n", version + 1);
        } else if (command == 1) {
            if (scanf("%f %f %f", &a, &b, &e) != 3) break;
            printf("Integral: %f\n", integral(a, b, e));
        } else if (command == 2) {
            if (scanf("%d %d", &x, &y) != 2) break;
            printf("Primes: %d\n", primes(x, y));
        }
    }

    dlclose(lib);
    return 0;
}