/*
Write a duplex pipe implementation.
typedef struct {
  int txd[2];
  int rxd[2];
} dpipe_t;
1) This pipe should connect some process with his child, for continuous communication.
2) Be careful with opened descriptors.
3) When one process is terminated, the other should also exit.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <malloc.h>


typedef struct {
  int txd[2];
  int rxd[2];
} dpipe_t;

int ReadStdin(char ** s) {
  char c = getchar();
  int i = 0;
  (*s) = (char *)malloc(sizeof(char));
  while (c != '\n') {
    (*s)[i] = c;
    c = getchar();
    i++;
    (*s) = (char *)realloc((*s), (i + 1) * sizeof(char));
  }
  (*s)[i] = '\0';
  return (i + 1);
}


int main() {
  dpipe_t dpip;
  pid_t pid;
  size_t cod;
  char * a, * b;
  int n1, n2, status;
  printf("What should parent send to child?\n");
  n1 = ReadStdin(&a);
  printf("What should child send to parent?\n");
  n2 = ReadStdin(&b);
  if ((pipe(dpip.txd) < 0) || (pipe(dpip.rxd) < 0)) {
    close(dpip.txd[0]);
    close(dpip.txd[1]);
    close(dpip.rxd[0]);
    close(dpip.rxd[1]);
    printf("Can't create pipe\n");
    free(a);
    free(b);
    exit(-1);
  }
  pid = fork();
  if (pid < 0) {
     close(dpip.txd[0]);
     close(dpip.txd[1]);
     close(dpip.rxd[0]);
     close(dpip.rxd[1]);
	   printf("Can't fork");
     free(a);
     free(b);
     exit(-1);
   }
   if (pid > 0) {
     close(dpip.txd[0]);
     close(dpip.rxd[1]);
     cod = write(dpip.txd[1], a, n1);
     if (cod != n1) {
       close(dpip.rxd[0]);
       close(dpip.txd[1]);
       printf("Error in parent's sending\n");
       free(a);
       free(b);
       exit(-1);
     }
     close(dpip.txd[1]);
     waitpid(pid, &status, 0);
     cod = read(dpip.rxd[0], b, n2);
     close(dpip.rxd[0]);
     if (cod != n2) {
       printf("Error in parent's reading\n");
       free(a);
       free(b);
       exit(-1);
     }
     printf("Parent has read %s\n", b);
     free(a);
     free(b);
     exit(0);
 } else if (pid == 0) {
   close(dpip.rxd[0]);
   close(dpip.txd[1]);
   cod = write(dpip.rxd[1], b, n2);
   close(dpip.rxd[1]);
   if (cod != n2) {
     close(dpip.txd[0]);
     printf("Error in child's sending\n");
     free(a);
     free(b);
     exit(-1);
   }
   cod = read(dpip.txd[0], a, n1);
   close(dpip.txd[0]);
   if (cod != n1) {
     printf("Error in child's reading\n");
     free(a);
     free(b);
     exit(-1);
   }
   printf("Child has read %s\n", a);
   free(a);
   free(b);
   exit(0);
 }
 free(a);
 free(b);
  return 0;
}
