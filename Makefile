#	TODO = https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#	TODo = https://makefiletutorial.com/

CC=gcc
#CFLAGS=-Wall -Wextra -Werror -Wstrict-prototypes -Wconversion -std=gnu11 \
#-O0 -g -ansi `pkg-config --cflags gtk+-3.0`
CFLAGS=-Werror -Wall -Wextra -Wconversion -std=gnu11

all: server client
.PHONY: all

server: server.o
	$(CC) -o server server.o

client: client.o
	$(CC) -o client client.o

clean:
	rm -f server client *.o