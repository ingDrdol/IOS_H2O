CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Werror -g

all: proj2

proj2: proj2.c makefile
	$(CC) $(CFLAGS) proj2.c -o proj2

clear: 
	rm *.o proj2
