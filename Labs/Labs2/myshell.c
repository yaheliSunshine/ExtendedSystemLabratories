#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include<signal.h>
#include "LineParser.h"
#define PATH_MAX        4096

int suspend(pid_t pid){
int sig = 0;
	if((sig=kill(pid,SIGTSTP))==-1){
		fprintf(stderr,"SIGTSTP didnt succeed");
	}
return sig;
} 

int wake(pid_t pid){
int sig = 0;
	if((sig=kill(pid,SIGCONT))==-1){
		fprintf(stderr,"SIGINT didnt succeed");
	}
return sig;

} 

int killl(pid_t pid){
int sig = 0;
	if((sig=kill(pid,SIGINT))==-1){
		fprintf(stderr,"SIGINT didnt succeed");
	}
return sig;

} 

cmdLine* redirect(cmdLine *pCmdLine,int debug){
if(pCmdLine->inputRedirect!=NULL){
	int fd = open(pCmdLine->inputRedirect,O_RDWR|O_CREAT);
	if(fd<0){
	fprintf(stderr,"inputredirect did not open");
	}
	
	dup2(fd,0);
}

if(pCmdLine->outputRedirect!=NULL){
	int fd1 = open(pCmdLine->outputRedirect,O_RDWR|O_CREAT,0777);
	if(fd1<0){
	fprintf(stderr,"inputredirect did not open");
	}
	dup2(fd1,1);
}
return pCmdLine;
}

void print_debug(cmdLine *pCmdLine){
	fprintf(stderr,"pid is:%d \n",getpid());
	fprintf(stderr,"executing command is:%s \n",(char*)pCmdLine->arguments[0]);
}

cmdLine* wait_pid(cmdLine *pCmdLine,pid_t pid, int debug){
	if(debug==1){
		print_debug(pCmdLine);
	}
	int status;
	if(pCmdLine->blocking!=0){
		waitpid(pid, &status, 0);
	}
return pCmdLine;
}

int execute(cmdLine *pCmdLine,int debug){
	if(debug==1){
		print_debug(pCmdLine);
	}
	redirect(pCmdLine,debug);
	execvp(pCmdLine->arguments[0],pCmdLine->arguments);
	perror("execvp failed!");
	freeCmdLines(pCmdLine);
	_exit(0);
}

int changeDir(char* buff,cmdLine *pCmdLine, int debug){
	if(debug==1){
		print_debug(pCmdLine);
	}
		int cd = chdir(buff);
	if(cd==-1){
		perror("change directory failed!!");
	}
return cd;	
}

char * getDir(char *buf, size_t size){
	if(getcwd(buf,size)==NULL){
		perror("get cwd didnt succeed!");
	}
	else{
		printf("the directiry is: %s \n",buf);
	}
return buf;
}
void quit(cmdLine *pCmdLine){
	freeCmdLines(pCmdLine);
	exit(0);
}


int main(int argc, char **argv)
{
char current_path[PATH_MAX];
char buff[2048];
int debug = 0;
int i = 0;
pid_t pid;
while(i<argc){
	
	if(strncmp(argv[i],"-D",2)==0){
		debug = 1;  
	}
	i++;
}

while(3){
	getDir(current_path,PATH_MAX);
	fgets(buff, 2048, stdin);

	cmdLine *pCmdLine = parseCmdLines(buff);
	if(!strncmp(pCmdLine->arguments[0],"quit",4)){
		quit(pCmdLine);
	}
	else if(!strncmp(pCmdLine->arguments[0],"cd",2)){
		changeDir(pCmdLine->arguments[1],pCmdLine,debug);
		freeCmdLines(pCmdLine);		
	}
	else if(!strncmp(pCmdLine->arguments[0],"suspend",7)){
		suspend(atoi(pCmdLine->arguments[1]));
		freeCmdLines(pCmdLine);
	}
	else if(!strncmp(pCmdLine->arguments[0],"wake",4)){
		wake(atoi(pCmdLine->arguments[1]));
		freeCmdLines(pCmdLine);
	}
	else if(!strncmp(pCmdLine->arguments[0],"kill",5)){
		killl(atoi(pCmdLine->arguments[1]));
		freeCmdLines(pCmdLine);
	}
	else {
	pid=fork();
	if(pid==-1){
		perror("fork failed!");
	}
	if(pid==0){
	execute(pCmdLine,debug);
	
	}
	else {
		wait_pid(pCmdLine,pid,debug);
		freeCmdLines(pCmdLine);
	}
	
   }
	
	
}





  
    return 0;
}
