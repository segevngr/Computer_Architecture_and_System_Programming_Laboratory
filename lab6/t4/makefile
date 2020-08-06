all: task4

task4: task4.o LineParser.o
	gcc -g -m32 -Wall -o task4 task4.o LineParser.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c -o LineParser.o LineParser.c 
	
task4.o: task4.c
	gcc -m32 -g -Wall -c -o task4.o task4.c 

.PHONY: clean
clean:
	rm -rf ./*.o task4
