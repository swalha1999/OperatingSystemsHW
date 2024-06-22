# CFLAGS = -g -Werror -pthread -std=c99 -o test
CFLAGS = -g -pthread -std=c99 
all: test.o
test.o: test.c concurrent_list.c concurrent_list.h
	gcc $(CFLAGS) test.c concurrent_list.c -o test
