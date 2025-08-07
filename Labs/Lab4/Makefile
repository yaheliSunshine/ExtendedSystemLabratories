all: task1 task4

task4: 
	gcc -m32 -g -Wall -fno-pie -fno-stack-protector task4.c -o task4 

task1: task1.o 
	gcc -m32 -g -Wall -o task1 task1.o 

task1.o: task1.c
	gcc -g -Wall -m32 -c -o task1.o task1.c


.PHONEY: clean

clean:
	rm -f *.o menu task4
