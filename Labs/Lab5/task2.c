#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "elf.h"
#include <sys/mman.h>
#include <sys/stat.h>
extern int startup(int argc, char **argv, void (*start)());
#define FALSE 0
#define TRUE 1

int TOGGLE = FALSE;

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
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void print_single_phdr(Elf32_Phdr *ptr, int x);
void print_single_phdr_L(Elf32_Phdr *ptr, int x);
char *string_flag_phdr(Elf32_Phdr *phdr); 
char *string_type_phdr(Elf32_Phdr *ptr);
void mmap_vis_flags(Elf32_Phdr *ehdr, int x);
void mmap_prot_flags(Elf32_Phdr *ehdr, int x);
void load_phdr(Elf32_Phdr *phdr, int fd);
int main(int argc, char **argv)
{
	int fd;
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		printf("\n\" %s \" could not open\n", argv[1]);
		exit(1);
	}

	struct stat statbuf;
	int err = fstat(fd, &statbuf);
	if (err < 0)
	{
		printf("\n\"%s \" could not open\n", argv[1]);
		exit(2);
	}

	void *map_ptr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map_ptr == MAP_FAILED)
	{
		printf("Mapping Failed\n");
		return 1;
	}
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_ptr;
	size_t progHedOffset = ehdr->e_phoff;
	size_t progHedNum = ehdr->e_phnum;
	/*applying for-each*/
	// foreach_phdr(map_ptr, print_single_phdr, 0);
	fprintf(stdout, "Type\tOffset\tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");

	foreach_phdr(map_ptr, print_single_phdr_L, 0); //task1a
	//foreach_phdr(map_ptr,mmap_prot_flags, 0);//task1b
	//foreach_phdr(map_ptr,mmap_vis_flags, 0);//task1b

	foreach_phdr(map_ptr, load_phdr, fd); //task2c
    startup(argc-1, argv+1, (void *)(ehdr->e_entry)); //task2c


}

void print_single_phdr(Elf32_Phdr *ptr, int i)
{
	fprintf(stdout, "Program header number %d at address %p\n", i, ptr);
	return;
}

void print_single_phdr_L(Elf32_Phdr *ptr, int x)
{
	// char str[100];
	// strcpy(str, );
	fprintf(stdout, "%s\t", string_type_phdr(ptr));
	fprintf(stdout, "%#08x\t", ptr->p_offset);
	fprintf(stdout, "%#10x\t", ptr->p_vaddr);
	fprintf(stdout, "%#10x\t", ptr->p_paddr);
	fprintf(stdout, "%#07x\t", ptr->p_filesz);
	fprintf(stdout, "%#07x\t", ptr->p_memsz);
	fprintf(stdout, "%s\t", string_flag_phdr(ptr));
	fprintf(stdout, "%#06x\n", ptr->p_align);
	// puts("kghhvjj");
	return;
}

char *string_flag_phdr(Elf32_Phdr *phdr) {
    if (phdr->p_flags == PF_X){
        return "X";
    }
    if (phdr->p_flags == PF_W) {
        return "W";
    }
    if (phdr->p_flags == PF_R) {
        return "R";
    }
    if (phdr->p_flags == PF_X + PF_W) {
        return "WX";
    }
    if (phdr->p_flags == PF_X + PF_R) {
        return "RX";
    }
    if (phdr->p_flags == PF_W + PF_R) {
        return "RW";
    }
    if (phdr->p_flags == PF_X + PF_W + PF_R) {
        return "RWX";
    }
    return "NONE";
}

char *string_type_phdr(Elf32_Phdr *ptr)
{
	if (ptr->p_type == PT_NULL)
		return "NULL";
	else if (ptr->p_type == PT_LOAD)
		return "LOAD";
	else if (ptr->p_type == PT_DYNAMIC)
		return "DYNAMIC";
	else if (ptr->p_type == PT_INTERP)
		return "INTERP";
	else if (ptr->p_type == PT_NOTE)
		return "NOTE";
	else if (ptr->p_type == PT_SHLIB)
		return "SHLIB";
	else if (ptr->p_type == PT_PHDR)
		return "PHDR";
	else if (ptr->p_type == PT_LOPROC)
		return "LOPROC";
	else if (ptr->p_type == PT_HIPROC)
		return "HIPROC";
	else
	{
		return "UNKNOWN";
	}
}
int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;

	size_t progHedOffset = ehdr->e_phoff;
	size_t progHedNum = ehdr->e_phnum;
	size_t i = 0;

	Elf32_Phdr *phdr_off = (Elf32_Phdr *)((char *)map_start + progHedOffset);
	while (i < progHedNum)
	{
		func(phdr_off, arg);
		phdr_off = (Elf32_Phdr *)((char *)phdr_off + ehdr->e_phentsize);
		i++;
	}
	return 0;
}

void mmap_prot_flags(Elf32_Phdr *ehdr, int x)
{
	int flag = 0;
	if (ehdr->p_flags & PF_R)
	{
		flag |= PROT_READ;
	}
	if (ehdr->p_flags & PF_W)
	{
		flag |= PROT_WRITE;
	}
	if (ehdr->p_flags & PF_X)
	{
		flag |= PROT_EXEC;
	}
	printf ("the prot flag is:%d\n" ,flag);
	return;
}

void mmap_vis_flags(Elf32_Phdr *ehdr, int x)
{
	//return (ehdr->p_flags & PF_W) ? (MAP_PRIVATE | MAP_SHARED) : MAP_PRIVATE;
	int mp = MAP_PRIVATE;
	if(ehdr->p_flags & PF_W)
	{
		printf ("the mmap flag is:%d\n" ,(mp|= MAP_SHARED));//(???)
	}
	else{
		printf ("the flag is:%d\n" ,(MAP_PRIVATE ));
	}
	return;
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{
	if (phdr->p_type == PT_LOAD){
		print_single_phdr_L(phdr, -1); // TOOD: remove int arg

		void *vadd = (phdr -> p_vaddr & 0xfffff000);
        int offset = phdr -> p_offset & 0xfffff000;
        int padding = phdr -> p_vaddr & 0xfff;

		int APPROPRIATE_PERMISSION_FLAGS = 0;
		if (phdr->p_flags & PF_R)
			APPROPRIATE_PERMISSION_FLAGS |= PROT_READ;
		if (phdr->p_flags & PF_W)
			APPROPRIATE_PERMISSION_FLAGS |= PROT_WRITE;
		if (phdr->p_flags & PF_X)
			APPROPRIATE_PERMISSION_FLAGS |= PROT_EXEC;

		int APPROPRIATE_MAPPING_FLAGS = (phdr->p_flags & PF_W) ? (MAP_PRIVATE | MAP_SHARED) : MAP_PRIVATE;

		if ((mmap(vadd, phdr->p_memsz+padding, APPROPRIATE_PERMISSION_FLAGS,  MAP_PRIVATE | MAP_FIXED , fd, offset)) == MAP_FAILED ) {
            perror("mmap failed!" );
	}
}
}