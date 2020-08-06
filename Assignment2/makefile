all: calc

calc: calc.o
	gcc -m32 -Wall -g calc.o -o calc

# .c/.s compile rules

calc.o: calc.s
	nasm -f elf calc.s -o calc.o

clean:
	rm calc calc.o
