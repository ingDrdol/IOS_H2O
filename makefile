CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g

all: proj2

proj2: proj2.o fifo.o fifo.h
	$(CC) proj2.o fifo.o -o proj2
proj2.o: proj2.c
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o
fifo.o: fifo.c
	$(CC) $(CFLAGS) -c fifo.c -o fifo.o

clear: 
	rm *.o proj2
