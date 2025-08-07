 	#include <stdio.h>
       #include <stdlib.h>
       #include <unistd.h>
       #include <string.h>
       #include <sys/types.h>
  	#include <sys/wait.h>

       int main(int argc, char *argv[]) //copied this code from man pipe
       {
           int pipefd[2];
           pid_t cpid;
           char buf;


           if (pipe(pipefd) == -1) {
               perror("pipe");
               exit(EXIT_FAILURE);
           }

           cpid = fork();
           if (cpid == -1) {
               perror("fork");
               exit(EXIT_FAILURE);
           }

           if (cpid > 0) {    /* parent reads from pipe */
               wait(NULL);                /* Wait for child */
               close(pipefd[1]);          /* Close unused write end */
		
               while (read(pipefd[0], &buf, 1) > 0)
                   write(STDOUT_FILENO, &buf, 1);


               close(pipefd[0]);
               
               _exit(EXIT_SUCCESS);

           } else {            /* Child writes "hello" to pipe */
               close(pipefd[0]);          /* Close unused read end */
               write(pipefd[1], "hello\n", strlen("hello\n"));
               close(pipefd[1]);          /* Reader will see EOF */
               
               exit(EXIT_SUCCESS);
           }
       }
