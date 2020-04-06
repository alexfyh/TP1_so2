#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

/*
	TODO = definir cómo recuperar y así la comparación de lo escrito y leido con la estrctura
*/
void send_mod(int32_t fd, const void *buf, size_t n, int32_t flags)
{
	ssize_t sended = send(fd, buf, n, flags);
	if (!(sended>0))
	{
		perror("Error en el escritura - socket");
		exit(EXIT_FAILURE);
	}
	return;
}

void recv_mod(int32_t fd, void *buf, size_t n, int32_t flags)
{
	ssize_t recieved = recv(fd, buf, n, flags);
	if (!(recieved>0))
	{
		perror("Error en la lectura - socket");
		exit(EXIT_FAILURE);
	}
	return;
}

void write_mod(int32_t fd, void *buf, size_t n)
{
	ssize_t written = write(fd, buf, n);
	if (!(written>0))
	{
		perror("Error en la escritura - pipe");
		exit(EXIT_FAILURE);
	}
	return;
}

void read_mod(int32_t fd, void *buf, size_t n)
{
	ssize_t readed = read(fd, buf, n);
	if (!(readed>0))
	{
		perror("Error en la lectura - pipe");
		exit(EXIT_FAILURE);
	}
	return;
}
