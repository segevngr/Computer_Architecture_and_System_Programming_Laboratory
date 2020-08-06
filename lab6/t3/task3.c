#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (int argc , char* argv[])
{
  int p[2];
  pipe(p);
  char *lsCommand[3] = { "ls", "-l", NULL };
  char *tailCommand[4] = {"tail", "-n", "2", NULL};
  int isDebug = 0;
  int child1, child2;

  if(argc > 1)
   if (strncmp(argv[1], "-d", 2) == 0)
     isDebug = 1;

  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>forking…");}
  
  // Child 1
  if((child1 = fork()) == 0)
  { 
    fclose(stdout);
    dup(p[1]);
    if(isDebug) {fprintf(stderr,"%s\n", "child1>redirecting stdout to the write end of the pipe…");}
    close(p[1]);
    if(isDebug) {fprintf(stderr,"%s\n", "child1>going to execute cmd: …");} 
    execvp(lsCommand[0], lsCommand);       
  }

  // Parent
  if(isDebug) {fprintf(stderr,"%s %d\n", "parent_process>created process with id: ", child1);}
  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>closing the write end of the pipe…");}
  close(p[1]);
  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>forking…");}

  //Child 2
  if((child2 = fork()) == 0) { 
    fclose(stdin);
    dup(p[0]);
    if(isDebug) {fprintf(stderr,"%s\n", "child2>redirecting stdout to the write end of the pipe…");}
    close(p[0]);
    if(isDebug) {fprintf(stderr,"%s\n", "child1>going to execute cmd: …");}
    execvp(tailCommand[0], tailCommand);
    if(isDebug) {fprintf(stderr,"%s\n", "parent_process>waiting for child processes to terminate…");}
  }

  // Parent
  if(isDebug) {fprintf(stderr,"%s %d\n", "parent_process>created process with id: ", child2);}
  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>closing the read end of the pipe…");}
  close(p[0]); 
  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>waiting for child processes to terminate…");}
  waitpid(child1, NULL, 0);
  waitpid(child2, NULL, 0);
  if(isDebug) {fprintf(stderr,"%s\n", "parent_process>exiting…");}
  return 0;
}