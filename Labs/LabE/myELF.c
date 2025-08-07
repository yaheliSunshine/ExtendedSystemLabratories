#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "elf.h"
#include <sys/mman.h>
#include <sys/stat.h>
#define FALSE 0
#define TRUE 1

int TOGGLE = FALSE;
int fd1 = -1;
int fd2 = -1;
struct stat statbuf_fd1;
struct stat statbuf_fd2;
char *map_ptr1;
char *map_ptr2;
char filename1[100];
char filename2[100];
struct fun_desc
{
	char *name;
	void (*fun)();
};

void toggleDebugMode();
void examineElfFile();
void printSectionNames();
Elf32_Shdr findSection(char *name, Elf32_Ehdr *ehdr_i);
char *findSectionNameforSym(Elf32_Sym *sym, size_t i, Elf32_Ehdr *ehdr_i);
void printSymbols();
void relocationTables();
void quit();
void CheckMerge();
void mergeFiles();


char *print_type(Elf32_Shdr *shdr_off);

struct fun_desc menu[] = {
	{"Toggle Debug Mode", toggleDebugMode},
	{"Examine Elf File", examineElfFile},
	{"Print Section Names", printSectionNames},
	{"Print Symbols", printSymbols},
	{"Relocation Tables", relocationTables},
	{"CheckMerge", CheckMerge},
	{"mergeFiles",mergeFiles},
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

	while (TRUE)
	{

		printf("Select operation from the following menu:\n ");

		int i = 0;
		while (i < 8)
		{
			printf("%d- %s\n ", i, menu[i].name);
			i++;
		}
		printf("option: ");
		int d;
		scanf("%d", &d);
		if (d > -1 && d < 8)
		{
			printf("within bounds\n");
			(menu[d].fun)();
		}
		else
		{
			printf("not within bounds\n");
		}
		// }
	}

	return 0;
}

void toggleDebugMode()
{
	TOGGLE = (!TOGGLE);
	fprintf(stderr, TOGGLE ? "Debug flag now on\n" : "Debug flag now off\n");
	return;
}
void examineElfFile()
{
	char filepath[100];
	fprintf(stdout, "ENTER ELF FILE:");
	scanf("%s", filepath);
	if (fd1 == -1)
	{
		fd1 = open(filepath, O_RDONLY);
		if (fd1 < 0)
		{
			printf("\n\"%s \" could not open\n",
				   filepath);
			exit(1);
		}
		strcpy(filename1, filepath);
		//struct stat statbuf_fd1;
		int err1 = fstat(fd1, &statbuf_fd1);
		if (err1 < 0)
		{
			printf("\n\"%s \" could not open\n",
				   filepath);
			exit(2);
		}

		map_ptr1 = mmap(NULL, statbuf_fd1.st_size,
						PROT_READ, MAP_SHARED,
						fd1, 0);
		if (map_ptr1 == MAP_FAILED)
		{
			printf("Mapping Failed\n");
			return;
		}

		Elf32_Ehdr *ehdr1 = (Elf32_Ehdr *)map_ptr1;
		fprintf(stdout, "mispar kesem: %x,%x,%x \n", ehdr1->e_ident[1], ehdr1->e_ident[2], ehdr1->e_ident[3]);

		if (strncmp((char *)(ehdr1->e_ident + 1), "ELF", 3) != 0)
		{
			printf("this is not ELF file");
			exit(0);
		}

		switch (ehdr1->e_ident[EI_DATA])
		{
		case ELFDATA2MSB:
			fprintf(stdout, "DATA: 2's complement, big endian\n");
			break;
		case ELFDATA2LSB:
			fprintf(stdout, "DATA: 2's complement, little endian\n");
			break;
		default:
			fprintf(stdout, "UNKNOWN\n");
			break;
		}

		fprintf(stdout, "nkodat ptiha: 0x%x \n", ehdr1->e_entry);
		fprintf(stdout, "The file offset in which the section header table resides: %d \n", ehdr1->e_shoff);
		fprintf(stdout, "The file offset in which the program header table resides: %d \n", ehdr1->e_phoff);

		fprintf(stdout, "The size of each program header entry: %d \n", ehdr1->e_phentsize);
		fprintf(stdout, "The number of program header entries: %d \n", ehdr1->e_phnum);

		fprintf(stdout, "The size of each section header entry: %d \n", ehdr1->e_shentsize);
		fprintf(stdout, "The number of section header entries: %d \n", ehdr1->e_shnum);
	}

	else if (fd2 == -1)
	{
		fd2 = open(filepath, O_RDONLY);
		if (fd2 < 0)
		{
			printf("\n\"%s \" could not open\n",
				   filepath);
			exit(1);
		}

		strcpy(filename2, filepath);

		//struct stat statbuf_fd2;
		int err2 = fstat(fd2, &statbuf_fd2);

		if (err2 < 0)
		{
			printf("\n\"%s \" could not open\n",
				   filepath);
			exit(2);
		}

		map_ptr2 = mmap(NULL, statbuf_fd2.st_size,
						PROT_READ, MAP_SHARED,
						fd2, 0);
		if (map_ptr2 == MAP_FAILED)
		{
			printf("Mapping Failed\n");
			return;
		}

		Elf32_Ehdr *ehdr2 = (Elf32_Ehdr *)map_ptr2;
		fprintf(stdout, "mispar kesem: %x,%x,%x \n", ehdr2->e_ident[1], ehdr2->e_ident[2], ehdr2->e_ident[3]);

		if (strncmp((char *)(ehdr2->e_ident + 1), "ELF", 3) != 0)
		{
			printf("this is not ELF file");
			exit(0);
		}

		switch (ehdr2->e_ident[EI_DATA])
		{
		case ELFDATA2MSB:
			fprintf(stdout, "DATA: 2's complement, big endian\n");
			break;
		case ELFDATA2LSB:
			fprintf(stdout, "DATA: 2's complement, little endian\n");
			break;
		default:
			fprintf(stdout, "UNKNOWN\n");
			break;
		}

		fprintf(stdout, "nkodat ptiha: 0x%x \n", ehdr2->e_entry);
		fprintf(stdout, "The file offset in which the section header table resides: %d \n", ehdr2->e_shoff);
		fprintf(stdout, "The file offset in which the program header table resides: %d \n", ehdr2->e_phoff);

		fprintf(stdout, "The size of each program header entry: %d \n", ehdr2->e_phentsize);
		fprintf(stdout, "The number of program header entries: %d \n", ehdr2->e_phnum);

		fprintf(stdout, "The size of each section header entry: %d \n", ehdr2->e_shentsize);
		fprintf(stdout, "The number of section header entries: %d \n", ehdr2->e_shnum);
	}
	else
	{
		perror("trying to load ELF fot 3rd time!");
	}
}

void printSectionNames()
{
	if (fd1 != -1)
	{
		fprintf(stdout, "FILE %s\n", filename1);
		Elf32_Ehdr *ehdr1 = (Elf32_Ehdr *)map_ptr1;

		size_t sectionOffset = ehdr1->e_shoff;
		size_t sectionNum = ehdr1->e_shnum;

		size_t i = 0;

		Elf32_Shdr *shdr_off = (Elf32_Shdr *)((char *)map_ptr1 + sectionOffset);

		Elf32_Shdr *sh_strtab = &shdr_off[ehdr1->e_shstrndx];

		const char *const sh_strtab_p = map_ptr1 + sh_strtab->sh_offset;
		while (i < sectionNum)
		{
			fprintf(stdout, "[%d]\t%s\t%s\t%x\t%x\t%x\n", i, sh_strtab_p + shdr_off->sh_name, print_type(shdr_off), shdr_off->sh_addr, shdr_off->sh_offset, shdr_off->sh_size);
			shdr_off = (Elf32_Shdr *)((char *)shdr_off + ehdr1->e_shentsize);
			i++;
		}
	}
	fprintf(stdout, "\n\n");
	if (fd2 != -1)
	{
		fprintf(stdout, "FILE %s\n", filename2);

		Elf32_Ehdr *ehdr2 = (Elf32_Ehdr *)map_ptr2;

		size_t sectionOffset = ehdr2->e_shoff;
		size_t sectionNum = ehdr2->e_shnum;

		size_t i = 0;

		Elf32_Shdr *shdr_off = (Elf32_Shdr *)((char *)map_ptr2 + sectionOffset);

		Elf32_Shdr *sh_strtab = &shdr_off[ehdr2->e_shstrndx];

		const char *const sh_strtab_p = map_ptr2 + sh_strtab->sh_offset;
		while (i < sectionNum)
		{
			fprintf(stdout, "[%d]\t%s\t%s\t%x\t%x\t%x\n", i, sh_strtab_p + shdr_off->sh_name, print_type(shdr_off), shdr_off->sh_addr, shdr_off->sh_offset, shdr_off->sh_size);
			shdr_off = (Elf32_Shdr *)((char *)shdr_off + ehdr2->e_shentsize);
			i++;
		}
	}
	if (fd1 == -1 && fd2 == -1)
	{
		perror("there are no loaded files!!");
	}
}

char *print_type(Elf32_Shdr *shdr_off)
{
	if (shdr_off->sh_type == SHT_NULL)
		return "NULL";

	else if (shdr_off->sh_type == SHT_PROGBITS)
		return "PROGBITS";

	else if (shdr_off->sh_type == SHT_SYMTAB)
		return "SYMTAB";

	else if (shdr_off->sh_type == SHT_STRTAB)
		return "STRTAB";

	else if (shdr_off->sh_type == SHT_RELA)
		return "RELA";

	else if (shdr_off->sh_type == SHT_HASH)
		return "HASH";

	else if (shdr_off->sh_type == SHT_DYNAMIC)
		return "DYNAMIC";

	else if (shdr_off->sh_type == SHT_NOTE)
		return "NOTE";

	else if (shdr_off->sh_type == SHT_NOBITS)
		return "NOBITS";

	else if (shdr_off->sh_type == SHT_REL)
		return "REL";

	else if (shdr_off->sh_type == SHT_SHLIB)
		return "SHLIB";

	else if (shdr_off->sh_type == SHT_DYNSYM)
		return "DYNSYM";

	else if (shdr_off->sh_type == SHT_LOPROC)
		return "LOPROC";

	else if (shdr_off->sh_type == SHT_HIPROC)
		return "HIPROC";

	else if (shdr_off->sh_type == SHT_LOUSER)
		return "LOUSER";

	else if (shdr_off->sh_type == SHT_HIUSER)
		return "SHT_HIUSER";

	else
	{
		return "UNknown";
	}
}
char *Section_name(Elf32_Shdr *strtab, Elf32_Word address, Elf32_Ehdr *ehdr_i)
{
	return ((char *)((void *)ehdr_i + strtab->sh_offset)) + address;
}

Elf32_Shdr findSection(char *name, Elf32_Ehdr *ehdr_i)
{
	//perror("in find section 1");
	Elf32_Shdr *shdr_i = (Elf32_Shdr *)((void *)ehdr_i + ehdr_i->e_shoff);
	//perror("in find section 2");
	Elf32_Shdr *strtab = (Elf32_Shdr *)&shdr_i[ehdr_i->e_shstrndx];
	//perror("in find section 3");
	for (size_t i = 0; i < ehdr_i->e_shnum; i++)
	{	
	//	perror("in find section in loop 4");
		if (strcmp(Section_name(strtab, shdr_i[i].sh_name, ehdr_i), name) == 0)
			return shdr_i[i];
	}
	return shdr_i[0];
}

char *findSectionNameforSym(Elf32_Sym *sym, size_t i, Elf32_Ehdr *ehdr_i)
{	//perror("in find section Name 1");
	Elf32_Shdr shstrtab = findSection(".shstrtab", ehdr_i);
	//perror("in find section Name 2");
	Elf32_Shdr *section = ((void *)ehdr_i) + ehdr_i->e_shoff + sym[i].st_shndx * ehdr_i->e_shentsize;
	//perror("in find section Name 3");
	return ((void *)ehdr_i) + shstrtab.sh_offset + section->sh_name;
}

void printSymbols()
{
	if (fd1 != -1)
	{
		fprintf(stdout, "FILE %s\n", filename1);
		Elf32_Ehdr *ehdr1 = (Elf32_Ehdr *)map_ptr1;

		Elf32_Shdr *shdr1 = (Elf32_Shdr *)((void *)ehdr1 + ehdr1->e_shoff);
		Elf32_Half shnum = ehdr1->e_shnum;
		//Elf32_Shdr *strtab = (Elf32_Shdr *)&shdr1[ehdr1->e_shstrndx];

		Elf32_Shdr *strtab = NULL;

		Elf32_Sym *symbolTable = NULL;

		 for (int  i = 0; i < ehdr1->e_shnum; i++) {

			Elf32_Shdr *currentSectionHeader = (Elf32_Shdr*)((void *)ehdr1 + ehdr1->e_shoff + i * ehdr1->e_shentsize);

			if(currentSectionHeader->sh_type == SHT_STRTAB){
            strtab= currentSectionHeader;
        	}

		 }


		for (Elf32_Half i = 0; i <= shnum; i++)
		{
			Elf32_Shdr curr = shdr1[i];
			if (curr.sh_type == 2)
			{
				size_t entriesNum = curr.sh_size / sizeof(Elf32_Sym);
				symbolTable = (void *)ehdr1 + curr.sh_offset;
				for (size_t j = 0; j < entriesNum; j++)
				{
					char *sectionName = symbolTable[j].st_shndx == 0xFFF1 ? "ABS" : symbolTable[j].st_shndx == 0x0 ? "UND"
																												   : findSectionNameforSym(symbolTable, j, ehdr1);
					if (TOGGLE)
					{
						fprintf(stderr, "[info] number of symbols: %d, st_name: %d, st_shndx: %d", symbolTable[j].st_size / sizeof(Elf32_Word), symbolTable[j].st_name, symbolTable[j].st_shndx);
					}
					char * bb = (char *)((void *)ehdr1) + strtab->sh_offset + symbolTable[j].st_name;
					printf("[%d]\t%x\t%d\t%s\t%s\n", j, symbolTable[j].st_value, symbolTable[j].st_shndx, sectionName, bb);
				}
			}
		}
	}
	fprintf(stdout, "\n\n");
	if (fd2 != -1)
	{
		fprintf(stdout, "FILE %s\n", filename2);
		Elf32_Ehdr *ehdr2 = (Elf32_Ehdr *)map_ptr2;

		Elf32_Shdr *shdr2 = (Elf32_Shdr *)((void *)ehdr2 + ehdr2->e_shoff);
		Elf32_Half shnum = ehdr2->e_shnum;
		//Elf32_Shdr *strtab = (Elf32_Shdr *)&shdr2[ehdr2->e_shstrndx];

				Elf32_Shdr *strtab = NULL;

		Elf32_Sym *symbolTable = NULL;

		 for (int  i = 0; i < ehdr2->e_shnum; i++) {

			Elf32_Shdr *currentSectionHeader = (Elf32_Shdr*)((void *)ehdr2 + ehdr2->e_shoff + i * ehdr2->e_shentsize);

			if(currentSectionHeader->sh_type == SHT_STRTAB){
            strtab= currentSectionHeader;
        	}

		 }


		for (Elf32_Half i = 0; i <= shnum; i++)
		{
			Elf32_Shdr curr = shdr2[i];
			if (curr.sh_type == 2)
			{
				size_t entriesNum = curr.sh_size / sizeof(Elf32_Sym);
				symbolTable = (void *)ehdr2 + curr.sh_offset;
				for (size_t j = 0; j < entriesNum; j++)
				{
					char *sectionName = symbolTable[j].st_shndx == 0xFFF1 ? "ABS" : symbolTable[j].st_shndx == 0x0 ? "UND"
																												   : findSectionNameforSym(symbolTable, j, ehdr2);
					if (TOGGLE)
					{
						fprintf(stderr, "[info] number of symbols: %d, st_name: %d, st_shndx: %d", symbolTable[j].st_size / sizeof(Elf32_Word), symbolTable[j].st_name, symbolTable[j].st_shndx);
					}

					char * bb = (char *)((void *)ehdr2) + strtab->sh_offset + symbolTable[j].st_name;

					printf("[%d]\t%x\t%d\t%s\t%s\n", j, symbolTable[j].st_value, symbolTable[j].st_shndx, sectionName, bb);
				}
			}
		}
	}
}
void relocationTables() {}
void quit()
{
	fprintf(stderr, "quitting\n");

	exit(0);
}

int countSymbolTables(Elf32_Ehdr *map_ptr)
{
	printf("countSymbolTables\n");

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_ptr;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((void *)ehdr + ehdr->e_shoff);
	Elf32_Half shnum = ehdr->e_shnum;

	Elf32_Shdr *strtab = (Elf32_Shdr *)&shdr[ehdr->e_shstrndx];
	int symbolTableCount = 0;

	for (Elf32_Half i = 0; i <= shnum; i++)
	{
		Elf32_Shdr curr = shdr[i];
		if (curr.sh_type == SHT_SYMTAB)
		{
			symbolTableCount++;
		}
	}
	return symbolTableCount;
}

int isValidElfFiles()
{
	printf("isValidElfFiles\n");

	// checks that 2 ELF files have been opened and mapped
	// returns 1 if valid, -1 else
	if (fd1 == -1 || fd2 == -1)
	{
		printf("ERROR: at least one file is closed or inaccessible\n");
		return -1;
	}

	if (map_ptr1 == MAP_FAILED || map_ptr2 == MAP_FAILED)
	{
		printf("ERROR: at least one elf hasn't benn mapped\n");
		return -1;
	}

	// each file contains exactly one symbol table
	// 	(otherwise, print "feature not supported" and return).
	if (countSymbolTables((Elf32_Ehdr *)map_ptr1) != 1 ||
		countSymbolTables((Elf32_Ehdr *)map_ptr2) != 1)
	{
		printf("feature not supported\n");
		return -1;
	}
	return 1;
}

Elf32_Sym *getSymTable(Elf32_Ehdr *map_ptr, size_t *symTableLen)
{
	printf("getSymTable\n");
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_ptr;
	Elf32_Shdr *shdr = (Elf32_Shdr *)((void *)ehdr + ehdr->e_shoff);
	Elf32_Half shnum = ehdr->e_shnum;

	Elf32_Shdr *strtab = (Elf32_Shdr *)&shdr[ehdr->e_shstrndx];
	Elf32_Sym *SYMTAB1 = NULL;

	for (Elf32_Half i = 0; i <= shnum; i++)
	{
		Elf32_Shdr curr = shdr[i];
		if (curr.sh_type == SHT_SYMTAB)
		{
			*symTableLen = curr.sh_size / sizeof(Elf32_Sym);
			SYMTAB1 = (void *)ehdr + curr.sh_offset;
			break;
		}
	}
	return SYMTAB1;
}

/*
first checks that 2 ELF files have been opened and mapped (print an error message and return otherwise).
NOTE: Here we assume that there are exactly 2 such ELF files, and each file contains exactly one symbol table
		(otherwise, print "feature not supported" and return).
then, for each ELF file, loops over all symbols (except for symbol number 0, which is a dummy symbol):
	in its symbol table SYMTAB1 and for each symbol sym:
		• If sym is UNDEFINED, search for sym in SYMTAB2, the symbol table of the other ELF file.
				If not found in SYMTAB2, or found in SYMTAB2 but UNDEFINED there as well, print an error message: "Symbol sym undefined".
		• If sym is defined, i.e. has a valid section number, again search for sym in SYMTAB2.
			In this case, if sym is found in SYMTAB2 amd is defined, print an error message: "Symbol sym multiply defined".

	Continue scanning symbols even after errors were found.
*/
void CheckMerge()
{
// using "F1a.o" and "F2a.o" should result in no errors, but using "F1b.o" and "F2b.o" should result in errors.
	if (isValidElfFiles() == -1)
	{
		return;
	}
	size_t symTableLen1Val;
	size_t *symTableLen1= &symTableLen1Val;

	Elf32_Ehdr *ehdr1 = (Elf32_Ehdr *)map_ptr1;
	Elf32_Ehdr *ehdr2 = (Elf32_Ehdr *)map_ptr2;

	Elf32_Sym *SYMTAB1 = getSymTable((Elf32_Ehdr *)map_ptr1, symTableLen1);

	//size_t *symTableLen2;
	size_t symTableLen2Val;
	size_t *symTableLen2= &symTableLen2Val;
	Elf32_Sym *SYMTAB2 = getSymTable((Elf32_Ehdr *)map_ptr2, symTableLen2);
	if (SYMTAB1 == NULL || SYMTAB2 == NULL){
		printf("null symtable\n");
	}

	Elf32_Shdr *strtab1 = NULL;

		

		 for (int  i = 0; i < ehdr1->e_shnum; i++) {

			Elf32_Shdr *currentSectionHeader = (Elf32_Shdr*)((void *)ehdr1 + ehdr1->e_shoff + i * ehdr1->e_shentsize);

			if(currentSectionHeader->sh_type == SHT_STRTAB){
            strtab1= currentSectionHeader;
        	}

		 }

	Elf32_Shdr *strtab2 = NULL;

		 for (int  i = 0; i < ehdr2->e_shnum; i++) {

			Elf32_Shdr *currentSectionHeader = (Elf32_Shdr*)((void *)ehdr2 + ehdr2->e_shoff + i * ehdr2->e_shentsize);

			if(currentSectionHeader->sh_type == SHT_STRTAB){
            strtab2= currentSectionHeader;
        	}

		 }	 

	for (size_t i = 1; i < *symTableLen1; i++)
	{	
		 char *name1 = strtab1 + SYMTAB1[i].st_name;
		if (strcmp(name1,"")==0)
            continue;
		if (SYMTAB1[i].st_shndx == SHN_UNDEF) // If sym is UNDEFINED..
		{
			int isValid = FALSE;
			//char *name1 = malloc(100*sizeof(char));
			//strcpy(name1, SYMTAB1[i].st_shndx == 0xFFF1 ? "ABS" : SYMTAB1[i].st_shndx == 0x0 ? "UND" :findSectionNameforSym(SYMTAB1, i, ehdr1));
			//// strcpy(name1, findSectionNameforSym(SYMTAB1, i, (Elf32_Ehdr *)map_ptr1));
			
			 int j;
			for (j = 1; j < *symTableLen2; j++)
			{	
				 char *name2 = strtab2 + SYMTAB2[j].st_name;
				//char *name2 = malloc(100*sizeof(char));
				//strcpy(name2, SYMTAB2[j].st_shndx == 0xFFF1 ? "ABS" : SYMTAB2[j].st_shndx == 0x0 ? "UND" :findSectionNameforSym(SYMTAB2, j, ehdr2));
				//// strcpy(name2, findSectionNameforSym(SYMTAB2, j, (Elf32_Ehdr *)map_ptr2));

				if (strcmp(name1, name2) == 0)
				{
					// found it in table2
					//if (SYMTAB2[j].st_shndx != SHN_UNDEF)
					//{
						// its defiend
						isValid = TRUE;
						break;

					//}
				}
		
			}

				if ((isValid==TRUE) || SYMTAB2[j].st_shndx == SHN_UNDEF)
			{
				fprintf(stderr,"Symbol %s undefined\n",name1);
			}
			//if (!isValid||SYMTAB2[j].st_shndx != SHN_UNDEF)
			//{
			//	fprintf(stderr,"Symbol sym undefined\n");
		//	}
		}
		if (SYMTAB1[i].st_shndx != SHN_UNDEF) // If sym is defined..
		{
			int isValid = FALSE;
			const char *name1 = strtab1 + SYMTAB1[i].st_name;
			//char *name1 = malloc(100*sizeof(char));
			//// strcpy(name1, findSectionNameforSym(SYMTAB1, i, (Elf32_Ehdr *)map_ptr1));
			//strcpy(name1, SYMTAB1[i].st_shndx == 0xFFF1 ? "ABS" : SYMTAB1[i].st_shndx == 0x0 ? "UNd" : findSectionNameforSym(SYMTAB1, i, ehdr1));

			size_t j;
			for (j = 1; j < *symTableLen2; j++)
			{	
				const char *name2 = strtab2 + SYMTAB2[j].st_name;
				//char *name2 = malloc(100*sizeof(char));
				//strcpy(name2, SYMTAB2[j].st_shndx == 0xFFF1 ? "ABS" : SYMTAB2[j].st_shndx == 0x0 ? "UND" : findSectionNameforSym(SYMTAB2, j, ehdr2));
				//// strcpy(name2, findSectionNameforSym(SYMTAB2, j, (Elf32_Ehdr *)map_ptr2));

				if (strcmp(name1, name2) == 0)
				{
					
					// found it in table2
					//if (SYMTAB2[j].st_shndx == SHN_UNDEF)
					//{
						// its undefiend
						isValid = TRUE;
						break;
						
					//}

				}
			
			}
			if (isValid&&SYMTAB2[j].st_shndx != SHN_UNDEF)
					{
						fprintf(stderr,"Symbol %s multiply defined\n",name1);
					}
				//if (!isValid||SYMTAB2[j].st_shndx == SHN_UNDEF)
				//	{
				//		fprintf(stderr,"Symbol sym multiply defined\n");
				//	}
			
			
		}
	}
	
	printf("checkMerge done\n");
	return;
}
void mergeFiles(){
	char* outMapPtr;
	int fdOut = open("out.ro",O_RDWR|O_CREAT);
	if(fdOut==-1){
		fprintf(stderr,"OPEM OUT EXE FAILED!");
		return;
	}
	off_t outStatSize = statbuf_fd1.st_size+statbuf_fd2.st_size;

	outMapPtr = mmap(NULL,outStatSize,PROT_WRITE|PROT_READ,MAP_SHARED,fdOut,0);
	if(outMapPtr==MAP_FAILED){
		fprintf(stderr,"mmap failed!");
	}
	Elf32_Ehdr *ehdrOut = (Elf32_Ehdr *)outMapPtr;
	Elf32_Shdr *shdrOut = (Elf32_Shdr *)((void *)ehdrOut + ehdrOut->e_shoff);

	Elf32_Ehdr *ehdr1 = (Elf32_Ehdr *)map_ptr1;
	Elf32_Shdr *shdr1 = (Elf32_Shdr *)((void *)ehdr1 + ehdr1->e_shoff);
	Elf32_Half shnum1 = ehdr1->e_shnum;

	Elf32_Ehdr *ehdr2 = (Elf32_Ehdr *)map_ptr2;
	Elf32_Shdr *shdr2 = (Elf32_Shdr *)((void *)ehdr2 + ehdr2->e_shoff);
	Elf32_Half shnum2 = ehdr2->e_shnum;



	memcpy(shdrOut, shdr1, ehdr1->e_shentsize * shnum1);

	Elf32_Shdr *strtab1 = NULL;
	Elf32_Sym *symbolTable1 = NULL;

	Elf32_Shdr *strtab2 = NULL;
	Elf32_Sym *symbolTable2 = NULL;

	 Elf32_Shdr* outMergeSection = NULL;

		 for (int  i = 0; i < ehdr1->e_shnum; i++) {
			Elf32_Shdr *currentSectionHeader = (Elf32_Shdr*)((void *)ehdr1 + ehdr1->e_shoff + i * ehdr1->e_shentsize);

			if(currentSectionHeader->sh_type == SHT_STRTAB){
            strtab1= currentSectionHeader;
        	}

			if(currentSectionHeader->sh_type == SHT_SYMTAB)
			symbolTable1= currentSectionHeader;
		 }

		  for (int  i = 0; i < ehdr1->e_shnum; i++) {
			Elf32_Shdr *currentSectionHeader2 = (Elf32_Shdr*)((void *)ehdr2 + ehdr2->e_shoff + i * ehdr2->e_shentsize);

			if(currentSectionHeader2->sh_type == SHT_STRTAB){
            strtab2= currentSectionHeader2;
        	}

			if(currentSectionHeader2->sh_type == SHT_SYMTAB)
			symbolTable2= currentSectionHeader2;
		 }

		 //char *sectionName = symbolTable[j].st_shndx == 0xFFF1 ? "ABS" : symbolTable[j].st_shndx == 0x0 ? "UNK"
				//																								   : findSectionNameforSym(symbolTable, j, ehdr2);
					


		 for (int  i = 0; i < ehdr1->e_shnum; i++) {
			Elf32_Shdr *currentSectionHeader1 = (Elf32_Shdr*)((void *)ehdr1 + ehdr1->e_shoff + i * ehdr1->e_shentsize);
			char *sectionName1 = symbolTable1[i].st_shndx == 0xFFF1 ? "ABS" : symbolTable1[i].st_shndx == 0x0 ? "UND"
																												   : findSectionNameforSym(symbolTable1, i, ehdr1);

				for (int  j = 0; j < ehdr2->e_shnum; j++) {
				Elf32_Shdr *currentSectionHeader2 = (Elf32_Shdr*)((void *)ehdr2 + ehdr2->e_shoff + j * ehdr2->e_shentsize);
				char *sectionName2 = symbolTable2[i].st_shndx == 0xFFF1 ? "ABS" : symbolTable2[i].st_shndx == 0x0 ? "UND"
																												   : findSectionNameforSym(symbolTable2, j, ehdr2);
				if((strcmp(sectionName1,".text")==0&&strcmp(sectionName2,".text")==0)||
					(strcmp(sectionName1,".data")==0&&strcmp(sectionName2,".data")==0)||
					(strcmp(sectionName1,".rodata")==0&&strcmp(sectionName2,".rodata")==0)){

						memcpy(outMergeSection,currentSectionHeader1,ehdr1->e_shentsize);
						memcpy(outMergeSection,currentSectionHeader2,ehdr2->e_shentsize);
				}	


			
		 }		
			
		 }

}