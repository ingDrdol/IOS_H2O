CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror -g

all: proj2

<<<<<<< HEAD
proj2: proj2.c makefile
	$(CC) $(CFLAGS) proj2.c -o proj2
=======
proj2: proj2.o fifo.o fifo.h
	$(CC) proj2.o fifo.o -o proj2
proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o
fifo.o: fifo.c
	$(CC) $(CFLAGS) -c fifo.c -o fifo.o
>>>>>>> cf80eaa5e9336355b0621db6320238291eebabd4

clear: 
	rm *.o proj2
