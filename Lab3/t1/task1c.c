#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    unsigned char* sig;
} virus;

virus *readVirus(FILE *fp) {
    unsigned short N = 0;
    char name[16];
    char NBuf[2];
    fread(NBuf, sizeof(char), 2, fp);
    N = *(unsigned short *) NBuf;
    fread(name, sizeof(char), 16, fp);
    char *signature = NULL;
    signature = malloc(sizeof(char) * N);
    fread(signature, sizeof(char), N, fp);

    virus *v = NULL;
    v = malloc(sizeof(virus));
    v->SigSize = N;
    v->sig = signature;
    for (int i = 0; i < 16; i++)
        v->virusName[i] = name[i];
    return v;
}

void printVirus(virus *virus, FILE *output) {
    fprintf(output, "Virus Name: %s\n", virus->virusName);
    fprintf(output, "Virus size: %d\n", virus->SigSize);
    fprintf(output, "Virus Signature:\n");
    for (int i = 0; i < virus->SigSize; i++)
        fprintf(output, "%02X ", virus->sig[i]);
    fprintf(output, "\n\n");
}

/********************************************/

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

void list_print(link *virus_list, FILE *listFile) {
    link *curLink = virus_list;
    while (curLink != NULL) {
        printVirus(curLink->vir, listFile);
        curLink = curLink->nextVirus;
    }
}

link *list_append(link *virus_list, virus *data) {
    link *newLink = malloc(sizeof(link));
    newLink->nextVirus = virus_list;
    newLink->vir = data;
    return newLink;
}

link *loadSignatures(link *virus_list) {
    int maxSize = 30;
    char input[maxSize];
    char fileName[maxSize];
    printf("Enter Signature file name \n");
    fgets(input, maxSize, stdin);
    sscanf(input, "%s", fileName);

    FILE *sigFile = NULL;
    sigFile = fopen(fileName, "r");
    virus *v;
    if(sigFile != NULL) {
        while (fgetc(sigFile) != EOF) {
            fseek(sigFile, -1, SEEK_CUR); // Go one character backwards
            v = readVirus(sigFile);
            virus_list = list_append(virus_list, v);
        }
    }
    fclose(sigFile);
    return virus_list;
}

void list_free(link *virus_list) {
    free(virus_list->vir);
    while(virus_list->nextVirus != NULL) {
        virus_list = virus_list->nextVirus;
        free(virus_list->vir);
    }
    free(virus_list);
}

link *quit(link *virus_list) {
    list_free(virus_list);
    exit(0);
}

/********************************************/
// Checks if a virus supplied in virus_list appears in
void detect_virus(char *buffer, unsigned int size, link *virus_list) {
    link *list = virus_list;
    while (list != NULL) {
        int sigSize = list->vir->SigSize;
        char *sig = list->vir->sig;
        for (int i = 0; i <= size - sigSize; i++) {
            if (memcmp((buffer + i), sig, sigSize) == 0) {
                printf("Virus starts in byte number %d \n", i);
                printf("Virus name is %s \n", list->vir->virusName);
                printf("Virus signature size is %d \n", list->vir->SigSize);
                printf("\n");
            }
        }
        list = list->nextVirus;
    }
}

/********************************************/

int main(int argc, char **argv) {
    link *virusList = NULL;
    char fileName = argv[1];

    FILE *output;
    output = fopen("output.txt", "w");

    char *menu[4] = {"Load signatures", "Print signatures", "Detect viruses", "Quit"};
    while (1) {
        printf("Choose an Option: \n");
        for (int i = 0; i < 4; i++)
            printf("%d) %s \n", i + 1, menu[i]);
        int maxSize = 30;
        char input[maxSize];
        char userInput[maxSize];
        fgets(input, maxSize, stdin);
        sscanf(input, "%s", userInput);
        int optionNum = atoi(userInput);

        if (optionNum < 1 || optionNum > 4)
            printf("Not within bounds \n");
        else {
            switch (optionNum) {
                case 1:     // Load Signatures
                {
                    virusList = loadSignatures(virusList);
                    break;
                }
                case 2:     // Print signatures
                {
                    list_print(virusList, output);
                    fclose(output);
                    break;
                }
                case 3:     // Detect Viruses
                {
                    FILE *infected;
                    infected = fopen(fileName, "r");
                    char buffer[10000];
                    fread(buffer, sizeof(char), 10000, infected);
                    fseek(infected, 0, SEEK_END);
                    unsigned int size = ftell(infected);
                    fseek(infected, 0, SEEK_SET);
                    if(size > 10000)
                        size = 10000;
                    detect_virus(buffer, size, virusList);
                    break;
                }
                case 4: {
                    quit(virusList);
                    break;
                }
            }
        }
    }
}
