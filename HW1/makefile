CC = gcc
CFLAGS = -Werror -std=c99

myshell: myshell.c
	$(CC) $(CFLAGS) -o myshell myshell.c

run: myshell
	./myshell

clean:
	rm -f myshell
