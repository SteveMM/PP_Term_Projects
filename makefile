main:	main.o union.o
	mpicc main.o union.o -o main

main.o:	main.c
	mpicc -c -std=c99 main.c

clean:	
	rm main.o union.o main
