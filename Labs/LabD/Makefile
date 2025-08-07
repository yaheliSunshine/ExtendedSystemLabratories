all: multi

multi: multi.o
	gcc -m32 -g -Wall -o multi multi.o

multi.o :  multi.s
	nasm -f elf32 multi.s -o multi.o
		
 .PHONEY: clean

clean:
	rm -f *.o multi
