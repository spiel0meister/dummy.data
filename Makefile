CC=gcc -std=c11
CFLAGS=-Wall -Wextra -ggdb

gen: gen.c
	$(CC) $(CFLAGS) -o $@ $<
