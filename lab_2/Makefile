all: main.out

main.out: main.c sort.c sort.h
	gcc -std=c11 -O2 -Wall -Wextra -pedantic -pthread -o main.out main.c sort.c

clean:
	rm -f main.out

.PHONY: all clean
