/*
  Write a program which allow you to:
- run another programs via command line.
- get exit codes of terminated programs

## TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (see "man exec").
   a) if execvp is used, string splitting into "path" and "args" is all you need.
3. Collect exit codes via waitpid/WEXITSTATUS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <malloc.h>

char ** a; // our argv

int StrToArgv(char * s) {
  int k = 0; // number of words (0 <-> 1)
  int j = 0, i = 0;
  a = (char **) malloc(sizeof(char *));
  while (s[i] != '\n') {
    a = (char **) realloc(a, (k + 1) * sizeof(char *)); // add memory for new word pointer
    a[k] = (char *) malloc(100); // memory for every single word
    j = 0;
    while (s[i] == ' ')
      i++;
    while ((s[i] != ' ') && (s[i] != '\n')) {
      a[k][j] = s[i];
      i++;
      j++;
    }
    a[k][j] = '\0';
    k++;
  }
  a = (char **) realloc(a, (k + 1) * sizeof(char *)); // add memory for NULL pointer
  a[k] = NULL;
  return k;
}

void clearargv(int NumberOfWords) {     // make memory free again
  for (int i = 0; i < NumberOfWords; i++) {
    free(a[i]);
  }
  free(a);
}

int main() {
  char str[256] = "";
  pid_t pid;
  fgets(str, 255, stdin);
  int n, status;
  while(strcmp(str, "quit\n") != 0) {
    n = StrToArgv(str);
    switch(pid = fork()) {
      case 0:                 //child code
        execv(a[0], a);
        printf("An error in execv\n"); //this code runs only in case of error
        exit(2);
        break;
      case -1: // error code
        printf("error, it isn't possible to create new process\n");
        break;
      default:
        waitpid(pid, &status, 0);                //parent code
        printf("Ret code: %d\n", WEXITSTATUS(status));
    }
    clearargv(n);
    fgets(str, 255, stdin);
  }
  return 0;
}
