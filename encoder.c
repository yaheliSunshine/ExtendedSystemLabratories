#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int length_Of(char* pArr){
 int l = 0;

    char* p = (pArr);
    while (*p != '\0'){
        ++p;
        l++;
        
    }
    return l;
}

int main(int argc , char* argv[]) {
FILE* fpIn = stdin;
FILE* fpOut = stdout;
int action = 0;
int i=0;
int placeOfNums = 0;
int length_encoder = 0;
int debugMode = 0;
	while(i<argc){
			if(strcmp(argv[i],"+D")==0){
				debugMode = 1; 
				fpOut = stderr;
				i=i+1;
			
			}
			else if(strcmp(argv[i],"-D")==0)
			{
				debugMode = 0;
				fpOut = stdout;
					i=i+1;
			
			}
			if(debugMode == 1){
			fprintf(fpOut," %s\n",argv[i]);
			}
		
			if(debugMode == 0){
				fprintf(fpOut," %s ",argv[i]);
			}
	
			if(strncmp(argv[i],"+e",2)==0){
			action = 1;
			placeOfNums = i;
			length_encoder = length_Of(argv[i]);
			
			}
			if(strncmp(argv[i],"-e",2)==0){
			action = -1;
			placeOfNums = i;
			length_encoder = length_Of(argv[i]);
			}
			if(strncmp(argv[i],"-i",2)==0){
			fpIn = fopen(argv[i]+2,"r+");
			}
			if(strncmp(argv[i],"-o",2)==0){
			fpOut = fopen(argv[i]+2,"w+");
			}
			
			
		i = i+1;
	}
	if(fpIn==NULL){
		fprintf(stderr," inputfile did not succeed \n");
	}
	if(fpOut==NULL){
		fprintf(stderr," outputfile did not succeed \n");
	}
	
	

int c;
int j=2;	
	
		while((c=fgetc(fpIn))!=EOF)
		{
			if(action==0){
				fputc(c,fpOut);
				}
			if(action!=0){
				
				if(c!='\n'){
					if(j>=length_encoder){
						j=2;
					}
					int pelet = c+action*(argv[placeOfNums][j]-'0');
					j = j+1;
						if(c>47 && c<58){
						if(pelet<=47){
						
							fputc(57-(47-pelet),fpOut);
						}
						if(pelet>=58){
							fputc(48+(pelet-58),fpOut);
						}
						if(pelet> 47 && pelet< 58){
							fputc(pelet,fpOut);
						}
					
					
					}
						if (c> 64 && c< 91){
							if(pelet<=64){
								fputc(90-(64-pelet),fpOut);
							}
							if(pelet>=91){
								fputc(65+(pelet-91),fpOut);
							}
							if(pelet> 64 && pelet< 91){
								fputc(pelet,fpOut);
							}
						
						}
					
						if (c> 96 && c< 123){
							if(pelet<=96){
							fputc(122-(96-pelet),fpOut);
						}
							if(pelet>=123){
								fputc(97+(pelet-123),fpOut);
							}
							if(pelet> 96 && pelet< 123){
								fputc(pelet,fpOut);
							}
							
							
							
						}
				
				}
			}		
		}
		if(fpIn!=stdout){
			fclose(fpOut);
		}
		if(fpIn!=stdin){
			fclose(fpIn);
		}
		exit(0);
			
		
	
	

	
}
