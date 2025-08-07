#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1

int TOGGLE = FALSE;

typedef struct
{
	char debug_mode;
	char file_name[128];
	int unit_size;
	unsigned char mem_buf[10000];
	size_t mem_count;

} state;

struct fun_desc
{
	char *name;
	void (*fun)(state*);
};

void toggleDebugMode();
void setFileName();
void setUnitSize();
void loaIntoMemory();
void toggleDisplayMode();
void memoryDisplay();
void saveIntoFile();
void memoryModify();
void quit();

struct fun_desc menu[] = {
	{"Toggle Debug Mode", toggleDebugMode},
	{"Set File Name", setFileName},
	{"Set Unit Size", setUnitSize},
	{"Load Into Memory", loaIntoMemory},
	{"Toggle Display Mode", toggleDisplayMode},
	{"Memory Display", memoryDisplay},
	{"Save Into File", saveIntoFile},
	{"Memory Modify", memoryModify},
	{"Quit ", quit},
	{NULL, NULL}};

char my_get(char c)
{
	return (char)fgetc(stdin);
}

char *map(char *array, int array_length, char (*f)(char))
{
	char *mapped_array = (char *)(malloc(array_length * sizeof(char)));

	for (int i = 0; i < array_length; i++)
	{
		*(mapped_array + i) = (*f)(*(array + i));
	}
	return mapped_array;
}

int main(int argc, char **argv)
{
	char str[10];
	state* currState  = malloc(sizeof(state));
	while (TRUE)
	{

		if (currState->debug_mode==1)
		{
			fprintf(stderr, "file_name: %s \nunit_size: %d \nmem_count: %d\n", currState->file_name, currState->unit_size, (int)currState->mem_count);
		}

		printf("Select operation from the following menu:\n ");

		int i = 0;
		while (i < 9)
		{
			printf("%d- %s\n ", i, menu[i].name);
			i++;
		}
		printf("option: ");
		// if (fgets(str, 10, stdin) == NULL)
		// {
		// 	exit(0);
		// }
		// else
		// {
			// int d = atoi(str);
			int d;
			scanf("%d", &d);
			if (d > -1 && d < 9)
			{
				printf("within bounds\n");
				(menu[d].fun)(currState);
			}
			else
			{
				printf("not within bounds\n");
			}
		// }
	}

	return 0;
}

void toggleDebugMode(state* s)
{
	s->debug_mode = (!s->debug_mode);
	fprintf(stderr,(s->debug_mode) ? "Debug flag now on\n" : "Debug flag now off\n");
	return;
}
void setFileName(state *s)
{
	char str[100];
	printf("Enter file name:\n");
	scanf("%s", str);
	strcpy(s->file_name,str);
	//fgets(str,100,stdin);
	//sscanf(str,"%s\n",s->file_name);
	if (s->debug_mode)
	{
		fprintf(stderr, "Debug: file name set to %s\n", s->file_name);
	}
	return;
}
void setUnitSize(state *s)
{
	/*
		1. Prompt the user for a number.
		2. If the value is valid (1, 2, or 4), set the size variable accordingly.
		3. If debug mode is on, print "Debug: set size to x", with x the appropriate size.
		4. If not valid, print an error message and leave size unchanged
	*/
	int unitSize;
	printf("Enter unit size:\n");
	scanf("%d", &unitSize);

	if (unitSize == 1 || unitSize == 2 || unitSize == 4)
	{
		s->unit_size = unitSize;
		fprintf(stderr, "Debug: set size to %d", s->unit_size);
	}
	else{
	printf("wrong unit size");
}
	return;
}
void loaIntoMemory(state* s)
{
	int location;
	int length;
	char buff1[60];
	FILE* fd; 
	if(strcmp(s->file_name,"")==0){
		fprintf(stderr,"ERROR: there is no file name!");
			return;
	}
	else{
		fd = fopen(s->file_name,"r+");
		if(fd == NULL){
			fprintf(stderr,"ERROR: open failed!");
			return;
		}
		else{
			printf("ENTER <LOCATION> AND <LENGTH>: ");
			scanf("%x %d",&location,&length);
			/*if(fgets (buff1, 20, stdin)==NULL){
				fprintf(stderr,"FGETS NOT WORKING!");
			}
			sscanf(buff1,"%x %d",&location,&length);
			*/
			if(s->debug_mode){
				fprintf(stderr, "file_name: %s \nlocation: %x \nlength: %d\n", s->file_name, location, length);
			}
			fseek(fd,location,SEEK_SET);
			fread(s->mem_buf,s->unit_size,length,fd);
			//fprintf(stderr,"!!!!%d!!!!!",(int)(s->mem_buf));
			s->mem_count = length*s->unit_size;
			fprintf(stderr,"Loaded %d units into memory\n",s->mem_count);
			fclose(fd);
		}
			



		}
	

	return;
}
void toggleDisplayMode(state* s)
{
	TOGGLE = (!TOGGLE);
	fprintf(stderr,TOGGLE ? "Display flag now on, hexadecimal representation\n" : "Display flag now off, decimal representation\n");
	return;
}
void memoryDisplay(state* s)
{	
	int addr;
	int u;
	unsigned int load1;
	int i=0;
	printf("Enter address and length: ");
	scanf("%x %d",&addr,&u);
	fprintf(stdout,TOGGLE ? "HexaDecimal\n" : "Decimal\n");
	fprintf(stdout,"=======\n");
	int l =s->unit_size*u;
	if(TOGGLE==1){
		while(i<u){
			//for(int j=0;j<s->unit_size;j++){
			//	load1 = (int)(s->mem_buf[j]);
			//}
			memcpy(&load1,s->mem_buf+addr,s->unit_size);
			printf("%#hx\n",load1);
			addr = addr+s->unit_size;
			i++;
		}
	}
	else{
		while(i<u){
			memcpy(&load1,s->mem_buf+addr,s->unit_size);
			printf("%#hd\n",load1);
			addr = addr+s->unit_size;
			i++;
		}
	}

	return;
} //0x80483b0
void saveIntoFile(state* s)
{	
	unsigned int sourceAddr,targetLocation;
	int length;
	FILE* fpTarget;
	printf("Please enter <source-address> <target-location> <length>");
	scanf("%x %x %d",&sourceAddr,&targetLocation,&length);
	fpTarget = fopen(s->file_name,"r+");
	if(fpTarget==NULL){
		fprintf(stderr,"file did not open!\n");
	}
//	if(targetLocation > (unsigned int)s->mem_count){
//		fprintf(stderr,"targetLocation > s->mem_count\n ");
//		return;
//	}
	if(s->debug_mode==1){
		fprintf(stderr, "source-address: %x \n targetlocation: %x \nlength: %d\n", sourceAddr, targetLocation, length);
	}
	fseek(fpTarget,targetLocation,SEEK_SET);
	//fwrite(,s->unit_size,length,fpTarget);
	if (sourceAddr == 0) 
	{
		fwrite(&(s->mem_buf), s->unit_size, length, fpTarget);
	}
    else {
		fwrite(&(sourceAddr), s->unit_size, length, fpTarget);
	}
    fclose(fpTarget);
	return;
}
void memoryModify(state* s)
{
	int location,val;
	printf("Please enter <location> <val>");
	scanf("%x %x",&location,&val);

	if(s->debug_mode==1){
		fprintf(stderr, "location: %x \nval: %d\n",location, val);
	}
	//FILE* fp = fopen(s->file_name,"r+");
	memcpy(s->mem_buf+location,&val,s->unit_size); // writing ok only when you are in the 1/4 bit
	//fwrite(s->mem_buf,s->unit_size,1,fp);

	return;
}
void quit(state* s)
{		
		free(s);
		fprintf(stderr, "quitting\n");

	exit(0);
}
