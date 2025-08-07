all: virusDetector

virusDetector: AntiVirus.c
	gcc -m32 -g -Wall -o virusDetector AntiVirus.c

.PHONEY: clean

clean:
	rm -f *.o virusDetector
