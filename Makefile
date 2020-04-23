CC=gcc
CFLAGS=-Werror -Wall -Wextra -Wconversion -std=gnu11 -Wpedantic -I . -g
DEPS = auth_definitions.h auth_functions.h state.h transactions.h client_functions.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server auth_service file_service client
.PHONY: all

server: server.o transactions.o
	$(CC) -o server server.o transactions.o

auth_service: auth_service.o auth_functions.o transactions.o
	$(CC) -o auth_service auth_service.o auth_functions.o transactions.o

client: client.o transactions.o client_functions.o
	$(CC) -o client client.o transactions.o client_functions.o file_functions.o -lcrypto -lssl

file_service: file_service.o file_functions.o file_functions.o transactions.o
	$(CC) -o file_service file_service.o file_functions.o transactions.o -lcrypto -lssl


clean:
	rm -f auth_service client server csv_handler file_service *.o