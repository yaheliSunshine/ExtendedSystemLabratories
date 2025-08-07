#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1

int digit_cnt(char* str){
int i=0;
int counter=0;
while(str[i]!='\0'){
	if(str[i]<='9'&&str[i]>='0'){
	counter++;
	}
i++;
}
return counter;
}

int main(int argc, char **argv)
{	
	digit_cnt(argv[1]);
	return 0;

}


