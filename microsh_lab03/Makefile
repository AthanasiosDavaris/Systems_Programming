CC=gcc
CFLAGS=-O2 -g -Wall -Wextra -Wpedantic

all: microsh

microsh: microsh.c
	$(CC) $(CFLAGS) -o $@ microsh.c

clean:
	rm -f microsh
