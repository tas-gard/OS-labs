all: lib1.so lib2.so program1.out program2.out

lib1.so: lib1.c functions.h
	gcc -std=c11 -Wall -Wextra -fPIC -shared lib1.c -lm -o lib1.so

lib2.so: lib2.c functions.h
	gcc -std=c11 -Wall -Wextra -fPIC -shared lib2.c -lm -o lib2.so

program1.out: program1.c functions.h lib1.so
	gcc -std=c11 -Wall -Wextra program1.c -L. -l1 -Wl,-rpath,'$$ORIGIN' -o program1.out

program2.out: program2.c lib1.so lib2.so
	gcc -std=c11 -Wall -Wextra program2.c -ldl -o program2.out

clean:
	rm -f *.so *.out

.PHONY: all clean