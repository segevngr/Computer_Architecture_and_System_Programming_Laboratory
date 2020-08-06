all: task2b

task2b: task2b.c task2b.o
	gcc -g -m32 -Wall -o task2b task2b.o

task2b.o: task2b.c
	gcc -m32 -g -Wall -c -o task2b.o task2b.c 

.PHONY: clean

clean: 
	rm -f *.o task2b
