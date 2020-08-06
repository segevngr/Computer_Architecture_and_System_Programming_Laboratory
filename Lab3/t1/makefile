all: task1c

task1c: task1c.c task1c.o
	gcc -g -m32 -Wall -o task1c task1c.o

task1c.o: task1c.c
	gcc -m32 -g -Wall -c -o task1c.o task1c.c 

.PHONY: clean

clean: 
	rm -f *.o task1c
