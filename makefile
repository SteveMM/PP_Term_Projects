main:	main.o
	mpicc main.o -o main -fopenmp

main.o:	main.c
	mpicc -c -std=c99 main.c

clean:	
	rm main.o main
