all: menu

menu: menumap.o 
	gcc -m32 -g -Wall -o menu menumap.o 

menumap.o: menumap.c
	gcc -g -Wall -m32 -c -o menumap.o menumap.c

.PHONEY: clean

clean:
	rm -f *.o menu
