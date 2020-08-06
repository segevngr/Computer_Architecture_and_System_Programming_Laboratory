all: task2

task2: task2.o LineParser.o
	gcc -g -m32 -Wall -o task2 task2.o LineParser.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
	
task2.o: task2.c
	gcc -m32 -g -Wall -c -o task2.o task2.c 

.PHONY: clean
clean:
	rm -rf ./*.o task2
