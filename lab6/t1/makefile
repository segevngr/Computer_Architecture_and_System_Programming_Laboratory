all: task1

task1: task1.o LineParser.o
	gcc -g -m32 -Wall -o task1 task1.o LineParser.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
	
task1.o: task1.c
	gcc -m32 -g -Wall -c -o task1.o task1.c 

.PHONY: clean
clean:
	rm -rf ./*.o task1
