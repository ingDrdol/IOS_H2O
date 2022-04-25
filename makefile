CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g

all: proj2

proj2: proj2.c
	$(CC) $(CFLAGS) proj2.c -o proj2

clear: 
	rm *.o proj2
