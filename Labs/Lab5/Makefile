all: my_loader

my_loader: task2.o start.o startup.o
	#gcc -m32 -g -Wall -o loader loader.o 
	ld -o my_loader task2.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

task2.o: task2.c
	gcc -g -m32 -Wall -c task2.c -o task2.o
	
start: start.o
	gcc -g -m32 -Wall -o start start.o

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s

startup: startup.o
	gcc -g -m32 -Wall -o startup startup.o

startup.o: startup.s
	nasm -g -f elf -w+all -o startup.o startup.s

.PHONEY: clean

clean:
	rm -f *.o my_loader
