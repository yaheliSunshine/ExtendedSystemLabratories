all: looper myshell 

myshell: myshell.o LineParser.o 
	gcc -m32 -g -Wall -o myshell LineParser.o myshell.o 

myshell.o: myshell.c 
	gcc -g -Wall -m32 -c -o myshell.o myshell.c

LineParser.o: LineParser.c LineParser.h
	gcc -g -Wall -m32 -c -o LineParser.o LineParser.c
		
looper: looper.c
	gcc -m32 -Wall -o looper looper.c

mypipe: mypipe.c
	gcc -m32 -g -Wall -o mypipe mypipe.c


.PHONEY: clean
clean:
	rm -f *.o myshell mypipe looper
