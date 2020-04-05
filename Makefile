#	TODO = https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
#	TODo = https://makefiletutorial.com/

CC=gcc
CFLAGS=-Werror -Wall -Wextra -Wconversion -std=gnu11 -I .
DEPS = auth_definitions.h auth_functions.h state.h transactions.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#CFLAGS=-Wall -Wextra -Werror -Wstrict-prototypes -Wconversion -std=gnu11 \
#-O0 -g -ansi `pkg-config --cflags gtk+-3.0`

all: server auth_service client
.PHONY: all

server: server.o transactions.o
	$(CC) -o server server.o transactions.o

auth_service: auth_service.o auth_functions.o
	$(CC) -o auth_service auth_service.o auth_functions.o

client: client.o transactions.o
	$(CC) -o client client.o transactions.o

clean:
	rm -f server client auth_service *.o