#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "LineParser.h"
#define PATH_MAX 4096
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0
#define HISTLEN 20


typedef struct process
{
	cmdLine *cmd;		  /* the parsed command line*/
	pid_t pid;			  /* the process id that is running the command*/
	int status;			  /* status of the process: RUNNING/SUSPENDED/TERMINATED */
	struct process *next; /* next process in chain */
} process;

process *process_list = NULL;
int newStatus;
void addProcess(process **list, cmdLine *cmd, pid_t pid)
{
	process *curr = (process *)malloc(sizeof(process));
	curr->cmd = cmd;
	curr->pid = pid;
	curr->status = RUNNING;
	if (*list == NULL)
	{
		curr->next = NULL;
		*list = curr;
	
	}
	else
	{
		curr->next = *list;
		*list = curr;
	}
}

void freeProcessList(process *process_list)
{
	process *temp = process_list;
	process *nextProc = NULL;
	while (temp != NULL)
	{
		if (temp->cmd)
			freeCmdLines(temp->cmd);
		nextProc = temp->next;
		if (temp)
			free(temp);
		temp = nextProc;
	}
}

void updateProcessStatus(process *list, int pid, int status)
{
	if (list == NULL)
	{
		fprintf(stderr, "list is NULL");
		_exit(EXIT_FAILURE);
	}
	process *curr = list;
	while (curr != NULL)
	{
		if (curr->pid == pid)
		{
			curr->status = status;
		}
		curr = curr->next;
	}
}

void updateProcessList(process **list)
{

	int updatedStatus = 0;
	pid_t rc_pid;
	if (*list == NULL)
	{
		fprintf(stderr, "list is NULL");
		_exit(EXIT_FAILURE);
	}
	process *cur = *list;
	while (cur != NULL)
	{
		rc_pid = waitpid(cur->pid, &updatedStatus, WNOHANG | WUNTRACED | WCONTINUED);
		if (rc_pid != 0)
		{
			if (WIFSIGNALED(updatedStatus) || WIFEXITED(updatedStatus))
			{
				updateProcessStatus(*list, cur->pid, TERMINATED);
				
			}

			else if (WIFSTOPPED(updatedStatus))
			{
				updateProcessStatus(*list, cur->pid, SUSPENDED);
				
			}
			else
			{
				updateProcessStatus(*list, cur->pid, RUNNING);
				
			}
		}
		cur = cur->next;
	}
}

void printProcessList(process **list)
{
	updateProcessList(list);
	int i = 0;
	char *strStatus;
	process *curr = *list;
	process *temp;
	process *prev;
	if (*list == NULL)
	{
		fprintf(stderr, "list is NULL");
	}
	fprintf(stdout, " PID				 COMMAND			status\n");
	while (curr != NULL)
	{
		if (curr->status == RUNNING)
		{
			strStatus = "RUNNING";
		}
		else if (curr->status == SUSPENDED)
		{
			strStatus = "SUSPENDED";
		}
		else
		{
			strStatus = "TERMINATED";
		}
		fprintf(stdout, " %d				 %s			%s\n", curr->pid, (char *)curr->cmd->arguments[0], strStatus);
		if ((*list)->status == TERMINATED)
		{
			temp = *list;
			curr = (*list)->next;
			(*list) = (*list)->next;
			freeCmdLines(temp->cmd);
			free(temp);
		}
		else if (curr->status == TERMINATED)
		{
			i = i + 1;
			temp = curr; // what we want to delete
			curr = curr->next;
			prev->next = curr; // chain together.
			freeCmdLines(temp->cmd);
			free(temp);
		}
		else
		{
			i = i + 1;
			prev = curr;
			curr = curr->next;
		}
	}
	
}
int suspend(pid_t pid)
{
	int sig = 0;
	if ((sig = kill(pid, SIGTSTP)) == -1)
	{
		fprintf(stderr, "SIGTSTP didnt succeed");
	}
	return sig;
}

int wake(pid_t pid)
{
	int sig = 0;
	if ((sig = kill(pid, SIGCONT)) == -1)
	{
		fprintf(stderr, "SIGINT didnt succeed");
	}
	return sig;
}

int killl(pid_t pid)
{
	int sig = 0;
	if ((sig = kill(pid, SIGINT)) == -1)
	{
		fprintf(stderr, "SIGINT didnt succeed");
	}
	return sig;
}

cmdLine *redirect(cmdLine *pCmdLine, int debug)
{
	if (pCmdLine->inputRedirect != NULL)
	{
		int fd = open(pCmdLine->inputRedirect, O_RDWR | O_CREAT);
		if (fd < 0)
		{
			fprintf(stderr, "inputredirect did not open");
		}

		dup2(fd, 0);
	}

	if (pCmdLine->outputRedirect != NULL)
	{
		int fd1 = open(pCmdLine->outputRedirect, O_RDWR | O_CREAT, 0777);
		if (fd1 < 0)
		{
			fprintf(stderr, "inputredirect did not open");
		}
		dup2(fd1, 1);
	}
	return pCmdLine;
}

void print_debug(cmdLine *pCmdLine)
{
	fprintf(stderr, "pid is:%d \n", getpid());
	fprintf(stderr, "executing command is:%s \n", (char *)pCmdLine->arguments[0]);
}

cmdLine *wait_pid(cmdLine *pCmdLine, pid_t pid, int debug)
{
	if (debug == 1)
	{
		print_debug(pCmdLine);
	}
	int status;
	if (pCmdLine->blocking != 0)
	{
		waitpid(pid, &status, 0);
	}
	return pCmdLine;
}
int simpleExecute(cmdLine *pCmdLine, int debug)
{
	if (debug == 1)
	{
		print_debug(pCmdLine);
	}
	redirect(pCmdLine, debug);
	execvp(pCmdLine->arguments[0], pCmdLine->arguments);
	perror("execvp failed!");
	freeCmdLines(pCmdLine);
	// _exit(0);
	return 1;
}

int execute(cmdLine *pCmdLine, int debug)
{
	if (debug == 1)
	{
		print_debug(pCmdLine);
	}
	if (pCmdLine->next == NULL)
	{
		pid_t pid = fork();

		
		if (pid == -1)
		{
			perror("fork failed!");
		}
		if (pid == 0)
		{
			
			if (simpleExecute(pCmdLine, debug) == 1){
				return 1;
			}
			
		}
		else
		{

			wait_pid(pCmdLine, pid, debug);
			addProcess(&process_list, pCmdLine, pid);
			
		}
	}
	else
	{

		int fd[2];
		if (pipe(fd) == -1)
		{
			fprintf(stderr, "pipe failed!");
			_exit(EXIT_FAILURE);
		}
		pid_t pid1;
		fprintf(stderr, "(parent_process>forking…)\n");
		if ((pid1 = fork()) == -1)
		{
			fprintf(stderr, "fork failed!");
			_exit(EXIT_FAILURE);
		}

		if (pid1 == 0)
		{
			fprintf(stderr, "(child1>redirecting stdout to the write end of the pipe…)\n");
			close(1);
			dup(fd[1]);
			close(fd[1]);
			fprintf(stderr, "(child2>going to execute cmd: …)\n");
			simpleExecute(pCmdLine, debug);
		}
		else
		{

			fprintf(stderr, "(parent_process>created process with id:%d)\n", pid1);
			fprintf(stderr, "(parent_process>closing the write end of the pipe…)\n");
			close(fd[1]);
		}

		pid_t pid2;
		fprintf(stderr, "(parent_process>forking…)\n");
		if ((pid2 = fork()) == -1)
		{
			fprintf(stderr, "fork failed!");
			_exit(EXIT_FAILURE);
		}

		if (pid2 == 0)
		{
			fprintf(stderr, "(child2>redirecting stdout to the write end of the pipe…)\n");
			close(0);
			dup(fd[0]);
			close(fd[0]);
			fprintf(stderr, "(child1>going to execute cmd: …)\n");
			simpleExecute(pCmdLine->next, debug);
		}
		else
		{
			fprintf(stderr, "(parent_process>created process with id:%d)\n", pid2);
			fprintf(stderr, "(parent_process>closing the read end of the pipe…)\n");
			close(fd[0]);
			
			fprintf(stderr, "(parent_process>waiting for child processes to terminate…)\n");
			wait_pid(pCmdLine, pid1, debug);
			wait_pid(pCmdLine->next, pid2, debug);
			addProcess(&process_list, pCmdLine, pid1);
			
			fprintf(stderr, "(parent_process>exiting…)\n");
		}
	}

	return 0;
}

int changeDir(char *buff, cmdLine *pCmdLine, int debug)
{
	if (debug == 1)
	{
		print_debug(pCmdLine);
	}
	int cd = chdir(buff);
	if (cd == -1)
	{
		perror("change directory failed!!");
	}
	return cd;
}

char *getDir(char *buf, size_t size)
{
	if (getcwd(buf, size) == NULL)
	{
		perror("get cwd didnt succeed!");
	}
	else
	{
		printf("the directiry is: %s \n", buf);
	}
	return buf;
}

int argc_hist = 0;
char **add_to_history(char **hist_arr, char *command, int *ptr_argc_hist)
{

	if ((*ptr_argc_hist) == HISTLEN)
	{ // delete and add
		for (int i = HISTLEN - 1; 0 <= i; i--)
		{
			*(hist_arr + i + 1) = strdup(*(hist_arr + i));
		}

		strcpy(hist_arr[0], command);
	}
	else
	{ // add and inc
		(*ptr_argc_hist)++;

		if ((*ptr_argc_hist) == 1)
		{
			hist_arr[0] = malloc(2048 * sizeof(char));
			strcpy(hist_arr[0], command);
		}
		else
		{
			hist_arr[(*ptr_argc_hist - 1)] = malloc(2048 * sizeof(char));
			int j;
			for (j = (*ptr_argc_hist) - 2; j > -1; j--)
			{

				strcpy(hist_arr[j + 1], hist_arr[j]);
			}
			strcpy(hist_arr[0], command);
		}
	}
	return hist_arr;
}
int exec_history(int digit, char **hist_arr, int debug)
{
	return execute(parseCmdLines(hist_arr[digit]), debug);
}

void print_history(char **hist_arr, int argc_hist)
{

	for (int i = 0; i < argc_hist; i++)
	{
		fprintf(stdout, "%d		%s\n", i, hist_arr[i]);
	}
}
void free_history(char **hist_arr)
{
	for (int i = 0; i < HISTLEN; i++)
		free(hist_arr[i]);
	free(hist_arr);
}
void quit(cmdLine *pCmdLine, process *list, char **hist_arr)
{
	freeProcessList(list);
	free_history(hist_arr); // make a function and UPDATE!!!
	freeCmdLines(pCmdLine);
	exit(0);
}

int main(int argc, char **argv)
{
	char current_path[PATH_MAX];
	char buff[2048];
	int debug = 0;
	int i = 0;
	char **hist_arr = (char **)malloc(HISTLEN * sizeof(char *));
	while (i < argc)
	{

		if (strncmp(argv[i], "-D", 2) == 0)
		{
			debug = 1;
		}
		i++;
	}

	while (3)
	{
		getDir(current_path, PATH_MAX);
		fgets(buff, 2048, stdin);

		cmdLine *pCmdLine = parseCmdLines(buff);
		if (!strncmp(pCmdLine->arguments[0], "quit", 4))
		{
			hist_arr = add_to_history(hist_arr, "quit", &argc_hist);
			quit(pCmdLine, process_list, hist_arr);
		}
		else if (!strncmp(pCmdLine->arguments[0], "cd", 2))
		{
			changeDir(pCmdLine->arguments[1], pCmdLine, debug);
			hist_arr = add_to_history(hist_arr, "cd", &argc_hist);
			freeCmdLines(pCmdLine);
		}
		else if (!strncmp(pCmdLine->arguments[0], "procs", 5))
		{
			// fprintf(stderr,"%p",(process_list));
			printProcessList(&process_list);
			hist_arr = add_to_history(hist_arr, "procs", &argc_hist);
			freeCmdLines(pCmdLine);
		}

		else if (!strncmp(pCmdLine->arguments[0], "suspend", 7))
		{
			suspend(atoi(pCmdLine->arguments[1]));
			freeCmdLines(pCmdLine);
		}
		else if (!strncmp(pCmdLine->arguments[0], "wake", 4))
		{
			wake(atoi(pCmdLine->arguments[1]));
			freeCmdLines(pCmdLine);
		}
		else if (!strncmp(pCmdLine->arguments[0], "kill", 5))
		{
			killl(atoi(pCmdLine->arguments[1]));
			freeCmdLines(pCmdLine);
		}
		else if (!strncmp(pCmdLine->arguments[0], "history", 7))
		{

			hist_arr = add_to_history(hist_arr, "history", &argc_hist);
			print_history(hist_arr, argc_hist);
			freeCmdLines(pCmdLine);
		}
		else if (!strncmp(pCmdLine->arguments[0], "!", 1))
		{ // need to check the line
			if (!strncmp(pCmdLine->arguments[0] + 1, "!", 1))
			{
				fprintf(stdout, "%s\n", hist_arr[0]);
				hist_arr = add_to_history(hist_arr, hist_arr[0], &argc_hist);
				exec_history(0, hist_arr, debug); // UPDATE
			}
			else
			{
				int n = atoi(pCmdLine->arguments[0] + 1); // need to check the line
				if (n == 0)
				{
					fprintf(stdout, "invalid argument!");
				}
				else
				{
					fprintf(stdout, "%s\n", hist_arr[n]);
					exec_history(n, hist_arr, debug); // UPDATE
					hist_arr = add_to_history(hist_arr, hist_arr[n + 1], &argc_hist);
				}
			}
			freeCmdLines(pCmdLine);
		}

		else
		{
			
			if (execute(pCmdLine, debug) == 0)
			{
				hist_arr = add_to_history(hist_arr, buff, &argc_hist);
			}
			// freeCmdLines(pCmdLine);
		}
	}

	return 0;
}
