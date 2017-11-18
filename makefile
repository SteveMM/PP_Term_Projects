main:	main.o
	gcc main.o -o main

main.o:	main.c
	gcc -c -std=c99 main.c

clean:	
rm main.o main
