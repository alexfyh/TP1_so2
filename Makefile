CC=gcc
#CFLAGS=-Wall -Wextra -Werror -Wstrict-prototypes -Wconversion -std=gnu11 \
#-O0 -g -ansi `pkg-config --cflags gtk+-3.0`
CFLAGS=-Werror -Wall -Wextra -Wconversion -std=gnu11
server: server.o
	$(CC) -o server server.o

#TODO = ver clean,y analizar si es necesario un .h y c para cada proceso