#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char censor(char c) {
    if(c == '!')
        return '.';
    else
        return c;
}

char* map(char *array, int array_length, char (*f) (char)){
    char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
    for(int i=0; i<array_length; i++)
        mapped_array[i] = (*f)(array[i]);
    return mapped_array;
}

/* Gets a char c and returns its encrypted form by adding 3 to its value.
          If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c) {
    if(0x20 <= c && 0x7E >= c)
        return c+3;
    return c;
}

/* Gets a char c and returns its decrypted form by reducing 3 to its value.
            If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c) {
    if(0x20 <= c && 0x7E >= c)
        return c-3;
    return c;}

/* dprt prints the value of c in a decimal representation followed by a
           new line, and returns c unchanged. */
char dprt(char c) {
    printf("%d \n", c);
    return c;

}

/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns
                    the value of c unchanged. */
char cprt(char c) {
    if (0x20 <= c && 0x7E >= c)
        printf("%c \n", c);
    else
        printf("%c \n", '.');
    return c;

}

/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c) {
    FILE *input = stdin;
    char tav = fgetc(input);
    return tav;
}

char quit(char c) {
    if(c == 'q')
        exit(0);
    return c;
}

int main () {
}


