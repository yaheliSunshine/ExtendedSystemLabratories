#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct fun_desc {
char *name;
char (*fun)(char);
}; 


char my_get(char c);
char cprt(char c);
char encrypt(char c);
char decrypt(char c);
char xprt(char c);

struct fun_desc menu[] = { { "Get string", my_get },{ "Print string", cprt },{ "Encrypt", encrypt }, 
			{ "Decrypt", decrypt }, { "Print Hex", xprt },{ NULL, NULL } }; 


char my_get(char c){
return (char)fgetc(stdin);
}

char cprt(char c){
int d = (int) c;
	if(d>=32 && d<=126){
		 printf("%c\n", c);
	}
	else{
		 printf("%c\n",'.');
	}
return c;
}

char encrypt(char c){
int d = (int) c;
	if(d>=32 && d<=126){
		return (char) (d+1);
	}
	else{
		return c;
	}
}

char decrypt(char c){
int d = (int) c;
	if(d>31 && d<127){
		return (char) (d-1);
	}
	else{
		return c;
	}
}

char xprt(char c){
	int d = (int) c;
	if(d>=32 && d<=126){
	printf("%x\n",c);
	return c;
	}
	else{
		printf("%c\n",'.');
		return c;
	}
}  
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
   
  	for(int i=0;i<array_length;i++){
  		*(mapped_array+i) =(*f)(*(array+i));
  	
  	}
  	return mapped_array;
}

int main(int argc, char **argv)
{ 
 int base_len = 5;
char* carray = (char*)(malloc(base_len*sizeof(char)));
char str[10];
	
	while(3){
	printf("Select operation from the following menu:\n ");
	
	
	
		int i=0;
		while(i<5){
		printf("%d) %s\n ", i ,menu[i].name);
		i++;
		}
		printf("option: ");
		if(fgets(str,10,stdin)==NULL){
				free(carray);
				exit(0);
		}
		else{
			int d = atoi(str);
			if(d>-1 && d<5){
				printf(" within bounds\n");
				 carray=map(carray,base_len,menu[d].fun);
				
				}
			else{
				printf("not within bounds\n");
			}
		
		}
		
		
	}
	return 0;
  }
  
