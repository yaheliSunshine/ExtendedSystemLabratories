#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
  unsigned short SigSize;
  char virusName[16];
  unsigned char* sig;
}
virus;

typedef struct link link;
struct link {
  link* nextVirus;
  virus* vir;
};

struct fun_desc {
  char* name;
  link* (* fun)(link *, char*);
};

int length_Of(int* pArr) {
  int l = 0;

  int* p = (pArr);
  while (* p != '\0') {
    ++p;
    l++;

  }
  return l;
}

void PrintHex(unsigned char* buff, short length, FILE* stream) {
  unsigned char* ptr = buff;
  int j = 0;
  while (j < length) {

    fprintf(stream, "%02X ",* ptr);
    ptr = ptr + 1;
    j = j + 1;
  }
	fprintf(stream, "\n\n");
}

virus* readVirus(FILE* stream) {
  unsigned char buff[18];
  virus* vir = (virus* ) malloc(sizeof(virus));
  if (!fread(buff, 18, 1, stream)) {
    free(vir);
    return NULL;
  }
  (* vir).SigSize = buff[0] + buff[1]* 256;

  strcpy((* vir).virusName, (char* ) buff + 2);
  vir -> sig = malloc(vir -> SigSize);
  fread(vir -> sig, 1, vir -> SigSize, stream);
  return vir;

}
void printVirus(virus* virus, FILE* stream) {
  fprintf(stream, "\nthe virus name: %s\n", (* virus).virusName);
  fprintf(stream, "the virus size :%d\n", (* virus).SigSize);
  PrintHex((* virus).sig, (* virus).SigSize, stream);
}

void list_print(link* virus_list, FILE* stream) {
  if (virus_list == NULL) {
    fprintf(stderr, "virus_list is NULL!");
  }
  link* current = virus_list;
  while (current != NULL) {
    printVirus((* current).vir, stream);
    current = (* current).nextVirus;
  }
}

int arr[10000];
int numOfViruses = 0;
void detect_virus(char* buffer, unsigned int size, link* virus_list) {
  int j = 0;
  while (virus_list != NULL) {
    for (int i = 0; i < size - virus_list->vir->SigSize; i++) {

      if (memcmp(buffer + i, virus_list->vir->sig, virus_list->vir->SigSize) == 0) {
        *(arr + j) = i;
		numOfViruses++;
        printf("the location starting point:%d\n", i);
        printf("the virus name:%s\n", (* (* (virus_list)).vir).virusName);
        printf("the virus size:%d\n", (* (* (virus_list)).vir).SigSize);

      }
    }
    virus_list = virus_list -> nextVirus;
  }
}

link* list_append(link* virus_list, virus* data) {

  link* current = (link* ) malloc(sizeof(link));
  if (current == NULL) {
    fprintf(stderr, "PROBLEM IN MALLOC!");
  }
  	(*current).vir = data;
	(*current).nextVirus = NULL;
  
  if (virus_list == NULL) {
	  return current;
  } else {
    (*current).nextVirus = virus_list;
  }
  return current;
}
void list_free(link* virus_list) {
  if (virus_list == NULL) {
    fprintf(stderr, "virus_list is NULL!");
  }
  link* current = virus_list;
  link* delete = NULL;
  while (current != NULL) {
    delete = current;
    current = (* current).nextVirus;
    free(delete -> vir -> sig);
    free(delete -> vir);
    free(delete);

  }
}

link* load_sigs(link* virus_list, char* fileName) {
	unsigned char buff2[100];
	char input[100];
	scanf("%s", input);
	FILE* fp = fopen(input, "rb");
  	if (fp == NULL) {
	exit(0);
	return 0;
	}
	fread(buff2, 4, 1, fp);
	if (buff2[3] == 'L' && buff2[2] == 'S' && buff2[1] == 'I' && buff2[0] == 'V') {
		virus* vir = NULL;
		while ((vir = readVirus(fp)) != NULL) {
		virus_list = list_append(virus_list, vir);

		}
	} else {
    	printf("ERRORFILE!");
    	exit(0);
  	}
  	fclose(fp);
	return virus_list;
}

link* print_sigs(link* virus_list, char* fileName) {
  list_print(virus_list, stdout);
  return virus_list;
}

link* detect_viruses(link* virus_list, char* fileName) {
	char buff[10000];
	FILE* infected_file = fopen(fileName, "rb+");
	if (infected_file == NULL) {
		printf("file didnt open");
	}		
	size_t elements_read = fread(buff, 1, 10000, infected_file);
	size_t num_bytes_read = (elements_read* sizeof(char));
	detect_virus(buff, (unsigned int) num_bytes_read, virus_list);
	fclose(infected_file);
  	return virus_list;
}

void neutralize_virus(char* fileName, int signatureOffset) {
  char patch[] = { 0xC3 };
  printf("killing a virus at location %d\n", signatureOffset);
  FILE* cp = fopen(fileName, "rb+");
  fseek(cp, signatureOffset, SEEK_SET);
  fwrite(patch, 1, 1, cp);
  fclose(cp);
}

link* fix_file(link* virus_list, char* fileName) {
	numOfViruses = 0;
	detect_viruses(virus_list, fileName);
	for (int k = 0; k < numOfViruses; k++) {
		neutralize_virus(fileName, *(arr + k));
	}
	numOfViruses = 0;
	return virus_list;
}

link* quit(link* virus_list, char* fileName) {
  list_free(virus_list);
  exit(0);
  return NULL;
}

struct fun_desc menu[] = {
  {"Load signatures", load_sigs }, { "Print signatures", print_sigs },{ "Detect viruses", detect_viruses }, { "Fix file", fix_file },
  { "Quit", quit }, { NULL, NULL } };

int main(int argc, char ** argv) {
  int base_len = 5;
  link* v_l = NULL;

    while (3) {
      printf("Select operation from the following menu:\n ");

      int i = 0;
      while (i < base_len) {
        printf("%d) %s\n ", i + 1, menu[i].name);
        i++;
      }
      printf("option: ");
      /*if(fgets(str,100,stdin)==NULL){
      		exit(0);
      }
      else{*/
      int d = 0; //atoi(str);
      if(scanf("%d", & d) == 0)
	  	quit(v_l, argv[1]);
      if (d > 0 && d < 6) {
          printf(" within bounds\n");
		  v_l = (menu[d - 1].fun)(v_l, argv[1]);
      } else {
        printf("not within bounds\n");
      }
      //}
    }
  //fclose(infected_file);

  return 0;
}