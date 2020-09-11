// Demonstrate use of fork

#include <stdio.h>
#include <stdlib.h>

// For strerror
#include <string.h>

// For fork, getpid, execve, read, write, close,
//   and other Linux system calls
#include <unistd.h>
// For errno
#include <errno.h>

const int MAX = 80;

int main ()
{
  char initString[MAX];
  printf ("Prompt> ");
  fgets(initString, MAX, stdin);
  int i = strcspn (initString, "\n");
  initString[strcspn (i, "\n")] = '\0';
  //fgets (initString, MAX, stdin); 

  pid_t pid = fork ();
  //Store calling func process id
  pid_t process_id;
  //Store parent func processid
  pid_t p_process_id;
  //getpid return process id of call
  process_id = getpid();
  //getpid return process id of parents
  p_process_id = getppid();
  
  if (pid < 0)
  {
    fprintf (stderr, "fork error (%s) -- exiting\n",
    strerror (errno));
    exit (-1);
  }

  if (pid == 0)
  {  
    printf("Child says: PID = %d, parent PID = %d, '%s'\n", process_id, p_process_id, initString);
    exit(0); 
  }

  printf("Parent says: PID = %d, child PID = %d, parent PID = %d\n", process_id, pid, p_process_id);

  
  return 0;
}
