all: task3

task3: task3.c task3.o
	gcc -g -m32 -Wall -o task3 task3.o

task3.o: task3.c
	gcc -m32 -g -Wall -c -o task3.o task3.c 

.PHONY: clean

clean: 
	rm -f *.o task3
