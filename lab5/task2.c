#include "LineParser.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h> 
#include "signal.h"


#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                        /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                /* next process in chain */
} process;


int debugFlag = 0;

char *statusToString(int status) {
  if (status == 1)
      return "RUNNING";
  else if (status == 0)
      return "SUSPENDED";
  else if (status == -1)
      return "TERMINATED";
}

void updateProcessStatus(process* process_list, int pid, int status){
  process* process_list_copy = process_list;
  while (process_list_copy->pid != pid)
    process_list_copy = process_list_copy->next;
  process_list_copy->status = status;
}

void updateStatus(process* currProcess,pid_t pid)
{
  int status;
  int retWait = waitpid(pid, &status, WNOHANG | WCONTINUED | WUNTRACED);
  if (retWait > 0) {
    if (WIFSTOPPED(status)) 
        updateProcessStatus(currProcess,pid,SUSPENDED);
    else if (WIFCONTINUED(status))
        updateProcessStatus(currProcess,pid,RUNNING);
    else if (WIFEXITED(status) || WIFSIGNALED(status))
        updateProcessStatus(currProcess,pid,TERMINATED);
  }
}

void updateProcessList(process **process_list){
  process* currProcess = process_list[0];
  while (currProcess != NULL) {
    updateStatus(currProcess,currProcess->pid);
    currProcess = currProcess->next;
  }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
  if (*process_list == NULL){ //empty list
    process* newProcess = (process*) malloc (sizeof(process));
    newProcess->cmd = cmd;
    newProcess->pid = pid;
    newProcess->status = RUNNING;
    newProcess->next = NULL;
    *process_list = newProcess;
  }
  else{
    addProcess(&((*process_list)->next), cmd, pid);
  }
}

void deleteTerminated(process** process_list)
{
  process* currProcess = process_list[0];
  process* prev = NULL;
  process* temp = NULL;
  while (currProcess != NULL){
    if (currProcess->status == TERMINATED)
    {
      if (prev == NULL)
        process_list[0] = currProcess->next;
      else{
        prev->next = currProcess->next;
      }
      temp = currProcess;
      temp->next = NULL;
      currProcess = currProcess->next;
      freeCmdLines(temp->cmd);
      free(temp); 
    }
    else {
      prev = currProcess;
      currProcess = currProcess->next;
    }
  }
}

void printProcessList(process** process_list){
  updateProcessList(process_list);
  printf("PID     Command     STATUS\n");
  process *currProcess = process_list[0];
  while (currProcess != NULL)
  {
    printf("%d      %s     %s\n", currProcess->pid,currProcess->cmd->arguments[0],statusToString(currProcess->status));
    currProcess = currProcess->next;
  }
  deleteTerminated(process_list);
}

void freeProcessList(process* process_list){
  while (process_list != NULL)
  {
    process* currProcess = process_list;
    process_list = process_list->next;
    freeCmdLines(currProcess->cmd);
    free(currProcess);
  }
}

void execute(cmdLine *pCmdLine)
{
  int exeRet = execvp(pCmdLine->arguments[0], pCmdLine->arguments);
  if (exeRet < 0){
    perror("error");
    exit(1);
  }
}

void printPath()
{
    char directPath[PATH_MAX];
    if (getcwd(directPath,PATH_MAX) != NULL) {
        printf("%s", directPath);
        printf("%s", ": ");
    }
}

int main (int argc , char* argv[], char* envp[])
{
  pid_t retFork;
  process** process_list = malloc(sizeof(struct process)); 
  *process_list = NULL; 
  int quitFlag = 0;
  int retKill;
  int SignaledPid;
  
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-D") == 0)
        debugFlag = 1;
  }

  while(quitFlag == 0){
    printPath(); // print the path in the beginning

    char inputLine[2048];
    fgets(inputLine,2048,stdin); // gets input from the user

    if (strncmp(inputLine,"quit",4) != 0){ // checks if the user asks to quit 
      cmdLine* command = parseCmdLines(inputLine);
      if (debugFlag == 1){
        fprintf(stderr,"PID: %d\n", retFork);
        fprintf(stderr,"Executing command: %s\n", command->arguments[0]);
      }
      
      if(strcmp(command->arguments[0], "procs") == 0){ // procs command 
        printProcessList(process_list);
        printf("%d      %s     %s\n", getpid(),command->arguments[0],"RUNNING");
      
      }
      else if(strcmp(command->arguments[0], "cd") == 0){ // cd command 
        int retChdir = chdir(command->arguments[1]);
        if (retChdir < 0){
          perror("error");
        }
      }
      else if(strcmp(command->arguments[0], "suspend") == 0){
        SignaledPid = atoi(command->arguments[1]);
        retKill = kill(SignaledPid,SIGTSTP);
        if (retKill<0)
          perror("signal error");         
      }
      else if(strcmp(command->arguments[0], "kill") == 0){
        SignaledPid = atoi(command->arguments[1]);
        retKill = kill(SignaledPid,SIGINT);
        if (retKill<0)
          perror("signal error");         
      }
      else if(strcmp(command->arguments[0], "wake") == 0){
        SignaledPid = atoi(command->arguments[1]);
        retKill = kill(SignaledPid,SIGCONT);
        if (retKill<0)
          perror("signal error");
      }
      else {
        int stat; // wait status
        retFork = fork();
        if (retFork == 0){ // child
          waitpid(retFork, &stat ,command->blocking); // blocking / non-blocking
          execute(command);
          retFork = getpid();
         }
      }
      if (retFork > 0)
        addProcess(process_list,command,retFork);
      retFork = -1;
    } 
    else {
      freeProcessList(*process_list);
      free (process_list);      
      quitFlag = 1;
    }
  }
  return 0;
}
