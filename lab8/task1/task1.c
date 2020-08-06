#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include<fcntl.h> 
#include<errno.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
    char debug_mode;
    int fd;
    void *map_start; /* will point to the start of the memory mapped file */
    int fd_size; /* this is needed to  the size of the file */
    Elf32_Ehdr *header; /* this will point to the header structure */
    int num_of_section_headers;
} elfFile;

struct fun_desc
{
    char *name;
    void (*fun)(elfFile *);
};

//*****************Task 0*******************//
void debugMode(elfFile *s)
{
    if (s->debug_mode == '1')
    {
        s->debug_mode = '0';
        printf("Debug flag now off \n");
    }
    else
    {
        s->debug_mode = '1';
        printf("Debug flag now on \n");
    }
}

void examineELFFile(elfFile *s)
{
    char input[100];
    printf("Enter file name \n");
    fgets(input, 100, stdin);
    char filename[128];
    sscanf(input, "%s", filename);
    if(s->debug_mode == '1')
    {
        fprintf(stderr, "Debug: Filename set to %s \n", filename);
    }
    s->fd = open(filename, O_RDONLY);
    if(s->fd < 0 ) { // opening the file
      perror("error in open");
      exit(-1);
    }
    s->fd_size = lseek(s->fd, 0, SEEK_END);
    lseek(s->fd, 0, SEEK_SET);  
    if ((s->map_start = mmap(0,s->fd_size, PROT_READ , MAP_SHARED, s->fd, 0)) == MAP_FAILED) { // mmap
        perror("mmap failed");
        exit(-4);
    }
    s->header = (Elf32_Ehdr *) s->map_start;
    s->num_of_section_headers = s->header->e_shnum;
    if (s->header->e_ident[EI_MAG1] != 'E' && s->header->e_ident[EI_MAG2] != 'L' && s->header->e_ident[EI_MAG3] != 'F')
    {
            perror("number is not consistent with an ELF file");
            exit(-1);
    }
    printf("Bytes 1,2,3 of the magic number: %c%c%c\n", s->header->e_ident[EI_MAG1],s->header->e_ident[EI_MAG2],s->header->e_ident[EI_MAG3]);
    if (s->header->e_ident[EI_DATA] == 1)
        printf("Data encoding scheme: 2's complement, little endian\n");
    else printf("Data encoding scheme: 2's complement, big endian\n");
    printf("Entry point: %X\n", s->header->e_entry);
    printf("Section headers offset: %d\n", s->header->e_shoff);
    printf("Number of section headers: %d\n", s->header->e_shnum);
    printf("Size of section headers: %d\n", s->header->e_shentsize);
    printf("Program headers offset: %d\n", s->header->e_phoff);
    printf("Number of program headers: %d\n", s->header->e_phnum);
    printf("Size of program headers: %d\n\n", s->header->e_phentsize);
}

void quit(elfFile *s)
{
    munmap(s->map_start, s->fd_size); /* unmap the file */
    close(s->fd);
    free(s);
    if(s->debug_mode == '1')
        fprintf(stderr, "quiting\n");
    exit(0);
}

//*****************Task 1*******************//

void printSectionNames(elfFile *s)
{
    Elf32_Shdr *sectionHeader = (Elf32_Shdr *)(s->map_start + s->header->e_shoff); // the section header
    int sectionHeaderNum = s->header->e_shnum;

    Elf32_Shdr * secHeadStringTable = &sectionHeader[s->header->e_shstrndx];
    char * stringTablePointer = s->map_start + secHeadStringTable->sh_offset;
    if(s->debug_mode == '0')
    {
        printf("[index]\tname\t\taddress offset\tsize\ttype\n");
        for (int i = 0; i < sectionHeaderNum; ++i) {
            printf("[%2d]%-20s%08X%5X%5X%15d\n", i, (stringTablePointer + sectionHeader[i].sh_name), sectionHeader[i].sh_addr, sectionHeader[i].sh_offset, sectionHeader[i].sh_size, sectionHeader[i].sh_type);
        }
    }
    else
    {
        printf("[index]\tname\t\taddress offset\tsize\ttype\t\tname offset\n");
        for (int i = 0; i < sectionHeaderNum; ++i) {
            printf("[%2d]%-20s%08X%5X%5X%15d%7X\n", i, (stringTablePointer + sectionHeader[i].sh_name), sectionHeader[i].sh_addr, sectionHeader[i].sh_offset, sectionHeader[i].sh_size, sectionHeader[i].sh_type, sectionHeader[i].sh_name);
        }
    }
}


int main(int argc, char **argv)
{
    int option;
    char input[10];
    elfFile *s = (elfFile *)malloc(sizeof(elfFile));
    s->debug_mode = '0';
    struct fun_desc menu[] = {{"Toggle Debug Mode", &debugMode},
                              {"Examine ELF File", &examineELFFile},
                              {"Print Section Names", &printSectionNames},
                              {"Quit", &quit},
                              {NULL, NULL}};
    while (1)
    {
        for (int i = 0; menu[i].name != NULL; i++)
            printf("%d) %s\n", i, menu[i].name);
        fgets(input, 10, stdin);
        option = atoi(input);
        if (0 <= option && option <= 3)
            menu[option].fun(s);
        else
            printf("Not Within bounds \n");
    }
}
