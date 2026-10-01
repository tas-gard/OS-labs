all: parent.out child.out

parent.out: parent.c
	gcc -std=c11 -Wall -Wextra -pedantic -o parent.out parent.c

child.out: child.c
	gcc -std=c11 -Wall -Wextra -pedantic -o child.out child.c

clean:
	rm -f parent.out child.out

.PHONY: all clean
