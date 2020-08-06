#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    int display_flag;
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
} state;

struct fun_desc
{
    char *name;
    void (*fun)(state *);
};

//*****************Task 0*******************//
void debugMode(state *s)
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

void setFileName(state *s)
{
    char input[100];
    printf("Enter file name \n");
    fgets(input, 100, stdin);
    sscanf(input, "%s", s->file_name);
    if(s->debug_mode == '1')
    {
        fprintf(stderr, "Debug: Filename set to %s \n", input);
    }
}

void setSize(state *s)
{
    char input[100];
    printf("Enter unit size \n");
    fgets(input, 100, stdin);
    int intSize = atoi(input);
    if (intSize == 1 || intSize == 2 || intSize == 4)
    {
        s->unit_size = intSize;
        if(s->debug_mode == '1')
            fprintf(stderr, "Debug: Unit size set to %d\n", intSize);
    }
    else
        perror("Unit size is not valid \n");
}

void quit(state *s)
{
    free(s);
    if(s->debug_mode == '1')
        fprintf(stderr, "quiting\n");
    exit(0);
}

//*****************Task 1a*****************//
void loadIntoMemory(state *s)
{
    char input[100];
    int loc;
    int len;

    if (s->file_name == NULL)
        perror("File is null \n");
    else
    {
        FILE *file = fopen(s->file_name, "r");
        if (file == NULL)
            perror("Failed to open file \n");
        else
        {
            printf("Enter file location \n");
            fgets(input, 100, stdin);
            sscanf(input, "%X", &loc);
            printf("Enter file length \n");
            fgets(input, 100, stdin);
            sscanf(input, "%d", &len);
            if(s->debug_mode == '1')
                fprintf(stderr, "Debug: File location: %X, File Length: %d\n", loc, len);
            fseek(file, loc, SEEK_SET);
            fread(s->mem_buf, s->unit_size, loc, file);
            printf("Loaded %d units into memory \n", len);
            fclose(file);
        }
    }
}

//****************Task 1b******************//
void toggleDisplayMode(state *s)
{
    if (s->display_flag == 1)
    {
        s->display_flag = 0;
        printf("Display flag now off, decimal representation \n");
    }
    else
    {
        s->display_flag = 1;
        printf("Display flag now on, hexadecimal representation \n");
    }
}

//****************Task 1c******************//
void memoryDisplay(state *s)
{

    char *hexadeximal[] = {"%02hhX\n", "%04hX\n", "No such unit", "%X\n"};
    char *decimal[] = {"%hhu\n", "%hu\n", "No such unit", "%u\n"};

    char input[10];
    char *format;
    int address;
    int length;
    unsigned char *start = s->mem_buf;
    unsigned char *end;

    printf("Enter address \n");
    fgets(input, 10, stdin);
    sscanf(input, "%X", &address);
    printf("Enter length \n");
    fgets(input, 10, stdin);
    sscanf(input, "%d", &length);

    if (s->display_flag == 0)
    {
        printf("Decimal \n");
        printf("======= \n");
        format = decimal[s->unit_size - 1];
    }
    else
    {
        printf("Hexadecimal \n");
        printf("=========== \n");
        format = hexadeximal[s->unit_size - 1];
    }

    if (address != 0)
        start = (unsigned char *)address;
    end = start + (length * (s->unit_size));

    while ((start < end))
    {
        if (s->unit_size == 1)
        {
            unsigned char var = *((unsigned char *)(start));
            fprintf(stdout, format, var);
            start += s->unit_size;
        }
        else if (s->unit_size == 2)
        {
            unsigned short var = *((unsigned short *)(start));
            fprintf(stdout, format, var);
            start += s->unit_size;
        }
        else
        { //unit size = 4
            unsigned int var = *((unsigned int *)(start));
            fprintf(stdout, format, var);
            start += s->unit_size;
        }
    }
}

//***************Task 1d******************//
void saveIntoFile(state *s)
{
    int target, source, len;
    char input[100000];
    unsigned char *buffer = s->mem_buf;

    printf("Enter source address \n");
    fgets(input, 100, stdin);
    sscanf(input, "%X", &source);
    if (source != 0)
        buffer = (unsigned char *)source;
    printf("Enter target location \n");
    fgets(input, 100000, stdin);
    sscanf(input, "%X", &target);
    printf("Enter length \n");
    fgets(input, 100000, stdin);
    sscanf(input, "%d", &len);
    if(s->debug_mode == '1')
        fprintf(stderr, "Debug: Source-address: %X, Location: %X, Length: %d\n", source, target, len);

    if (s->file_name == NULL)
        perror("File is null \n");
    else
    {
        FILE *file = fopen(s->file_name, "r+");
        if (file == NULL)
            perror("Failed to open file \n");
        else
        {
            fseek(file, 0, SEEK_END);
            int fileSize = ftell(file);
            if (fileSize < target)
                perror("Target exceeds file size \n");
            else
            {
                fseek(file, target, SEEK_SET);
                fwrite(buffer, s->unit_size, len, file);
                fclose(file);
            }
        }
    }
}

//***************Task 1e******************//
void memoryModifiy(state *s)
{
    unsigned char *buffer = s->mem_buf;
    char input[100];
    int location;
    int val;

    printf("Enter location \n");
    fgets(input, 100, stdin);
    sscanf(input, "%X", &location);
    printf("Enter value \n");
    fgets(input, 100, stdin);
    sscanf(input, "%X", &val);
    if(s->debug_mode == '1')
        fprintf(stderr, "Debug: Location: %X, Value: %X\n",location, val);

    if (s->unit_size == 1)
        *(unsigned char *)(buffer + location) = (unsigned char)val;
    else if (s->unit_size == 2)
        *(unsigned short *)(buffer + location) = (unsigned short)val;
    else
        *(unsigned int *)(buffer + location) = (unsigned int)val;
}

int main(int argc, char **argv)
{
    int option;
    char input[10];
    state *s = (state *)malloc(sizeof(state));
    s->unit_size = 1;
    s->debug_mode = '0';
    struct fun_desc menu[] = {{"Toggle Debug Mode", &debugMode},
                              {"Set File Name", &setFileName},
                              {"Set Unit Size", &setSize},
                              {"Load Memory", &loadIntoMemory},
                              {"Toggle Display Mode", &toggleDisplayMode},
                              {"Memory Display", &memoryDisplay},
                              {"Save Into File", &saveIntoFile},
                              {"Memory Modify", &memoryModifiy},
                              {"Quit", &quit},
                              {NULL, NULL}};
    while (1)
    {
        if(s->debug_mode == '1')
            fprintf(stderr, "Debug: unit_size = %d, file_name = %s, mem_count = %d\n", s->unit_size, s->file_name, s->mem_count);
        for (int i = 0; menu[i].name != NULL; i++)
            printf("%d) %s\n", i, menu[i].name);
        fgets(input, 10, stdin);
        option = atoi(input);
        if (0 <= option && option <= 9)
            menu[option].fun(s);
        else
            printf("Not Within bounds \n");
    }
}
