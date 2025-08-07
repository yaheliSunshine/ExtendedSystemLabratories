#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>

int main(int argc, char **argv)
{
int fd[2];
	if(pipe(fd)==-1){
	fprintf(stderr,"pipe failed!");
	_exit(EXIT_FAILURE);
}
pid_t pid1;
	fprintf(stderr,"parent_process>forking…\n");
	if ((pid1 = fork())==-1){
	fprintf(stderr,"fork failed!");
	_exit(EXIT_FAILURE);
	}
	
	if(pid1==0){
		fprintf(stderr,"(child1>redirecting stdout to the write end of the pipe…)\n");
		close(1);
		dup(fd[1]);
		close(fd[1]);
		char* args1[] = {"ls","-l",0};
		fprintf(stderr,"(child2>going to execute cmd: …)\n");
		execvp(args1[0],args1);
		perror("execvp failed!");
		_exit(EXIT_FAILURE);
		
	}	
else{
	
	fprintf(stderr,"parent_process>created process with id:%d\n",pid1);
	fprintf(stderr,"parent_process>closing the write end of the pipe…\n");
	close(fd[1]);
}

pid_t pid2;
	fprintf(stderr,"parent_process>forking…\n");
	if ((pid2 = fork())==-1){
		fprintf(stderr,"fork failed!");
		_exit(EXIT_FAILURE);
		}
		
	if(pid2==0){
		 fprintf(stderr,"(child2>redirecting stdout to the write end of the pipe…)\n");
		close(0);
		dup(fd[0]);
		close(fd[0]);
		fprintf(stderr,"(child1>going to execute cmd: …)\n");
		char* args2[] = {"tail","-n","2",0};
		execvp(args2[0],args2);
		perror("execvp failed!");
		_exit(EXIT_FAILURE);
	}
else{
	fprintf(stderr,"parent_process>created process with id:%d\n",pid2);
	fprintf(stderr,"parent_process>closing the read end of the pipe…\n");
	close(fd[0]);
	int status1,status2;
	fprintf(stderr,"parent_process>waiting for child processes to terminate…\n");
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	fprintf(stderr,"(parent_process>exiting…)\n");
	_exit(0);
}	


	  
    return 0;
}
