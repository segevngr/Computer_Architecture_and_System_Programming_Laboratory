#include "util.h"

#define SYS_CLOSE 6
#define SYS_OPEN 5
#define SYS_WRITE 4
#define SYS_READ 3
#define SYS_LSEEK 19
#define SYS_EXIT 1
#define SEEK_SET 0
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define ENOENT 2
#define O_CREATE 64
#define O_WRONLY 1
#define O_RDONLY 0

extern int system_call();

int open_debug_system_call(int debugFlag, char* path, int flag, int permission)
{
  int fileNum = system_call(SYS_OPEN,path,flag,permission);
  if(debugFlag == 1){
    char * str;
    str = itoa(fileNum);
    system_call(SYS_WRITE,STDERR,path,strlen(path));
    system_call(SYS_WRITE,STDERR,"\n",1);
    system_call(SYS_WRITE,STDERR,"5 ",2);
    system_call(SYS_WRITE,STDERR,str,strlen(str));
    system_call(SYS_WRITE,STDERR,"\n",1);
  }
  return fileNum;
}

int write_debug_system_call(int debugFlag, int fileNum, char* buffer, int length)
{
  int bytesSent = system_call(SYS_WRITE, fileNum, buffer, length);
  if(debugFlag == 1){
    char * str;
    str = itoa(bytesSent);
    system_call(SYS_WRITE,STDERR,"4 ",2);
    system_call(SYS_WRITE,STDERR,str,strlen(str));
    system_call(SYS_WRITE,STDERR,"\n",1);
  }
  return bytesSent;
}

int read_debug_system_call(int debugFlag, int fileNum, char* buffer, int length)
{
  int bytesRead = system_call(SYS_READ, fileNum, buffer, length);
  if(debugFlag == 1){
    char * str;
    str = itoa(bytesRead);
    system_call(SYS_WRITE,STDERR,"3 ",2);
    system_call(SYS_WRITE,STDERR,str,strlen(str));
    system_call(SYS_WRITE,STDERR,"\n",1);
  }
  return bytesRead;
}

int close_debug_system_call(int debugFlag, int fileNum)
{
  int returnedVal =   system_call(SYS_CLOSE,fileNum);
  if(debugFlag == 1){
    char * str;
    str = itoa(returnedVal);
    system_call(SYS_WRITE,STDERR,"6 ",2);
    system_call(SYS_WRITE,STDERR,str,strlen(str));
    system_call(SYS_WRITE,STDERR,"\n",1);
  }
  return fileNum;
} 

int main (int argc , char* argv[], char* envp[])
{
  int i = 0;
  int debugFlag = 0;
  int outputFlag = 0;
  int outputArg = 0;
  int outputFileNum = -1;
  int inputFlag = 0;
  int inputArg = 0;
  int inputFileNum = -1;
  char c[1];

  for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-o",2) == 0) {
            outputFlag = 1;
            outputArg = i;
        }
        if (strncmp(argv[i], "-i",2) == 0) {
            inputFlag = 1;
            inputArg = i;
        }
        if (strcmp(argv[i], "-D") == 0)
            debugFlag = 1;
  }

  if (outputFlag == 1)
    outputFileNum = open_debug_system_call(debugFlag,(argv[outputArg] + 2),O_CREATE | O_WRONLY,0777);
  else {
    outputFileNum = STDOUT;
    if (debugFlag == 1)
      system_call(SYS_WRITE,STDERR,"STDOUT\n",7);
  }

  if (inputFlag == 1){
    inputFileNum = open_debug_system_call(debugFlag,(argv[inputArg] + 2),O_RDONLY,0777);
    if (inputFileNum < 0)
      {
        char* err = "Can't open input file\n";
        write_debug_system_call(debugFlag,STDERR,err,strlen(err));
        system_call(SYS_EXIT,ENOENT);
      }
  }
  else {
    inputFileNum = STDIN;
        if (debugFlag == 1)
      system_call(SYS_WRITE,STDERR,"STDIN\n",6);
  }
  int bytesReaded = read_debug_system_call(debugFlag,inputFileNum,c,1);

  while (bytesReaded > 0 && c[0] != '\n') {
    if (c[0] >= 'a' && c[0] <= 'z')
    {
        c[0] =c[0]-('a'-'A');
        write_debug_system_call(debugFlag,outputFileNum,c,1);
    } else
        write_debug_system_call(debugFlag,outputFileNum,c,1);

    bytesReaded = read_debug_system_call(debugFlag,inputFileNum, c,1);
  }

  if(outputFlag == 0)
    write_debug_system_call(debugFlag,STDOUT,"\n",1);
  
  close_debug_system_call(debugFlag,outputFileNum);
  close_debug_system_call(debugFlag,inputFileNum);

  return 0;
}
