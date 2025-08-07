all: task0

task0: start.o 
	ld -m elf_i386 start.o -o task0

start.o :  start.s
	nasm -f elf32 start.s -o start.o
	
 	
 .PHONEY: clean

clean:
	rm -f *.o task0	
