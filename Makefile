CC=gcc
CFLAGS=-Werror -Wall -Wextra -Wconversion -std=gnu11 -I .
DEPS = auth_definitions.h auth_functions.h state.h transactions.h 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

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