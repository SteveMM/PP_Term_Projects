main:	main.o union.o
	mpicc main.o union.o -o main

union.o:	union.c union.h
	mpicc -c std=c99 union.c

main.o:	main.c
	mpicc -c -std=c99 main.c

clean:	
	rm main.o union.o main
