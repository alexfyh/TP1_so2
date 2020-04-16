#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

/*
	TODO = definir cómo recuperar y así la comparación de lo escrito y leido con la estrctura
*/
void send_mod(int32_t fd, const void *buf, size_t n, int32_t flags)
{
	ssize_t sended = send(fd, buf, n, flags);
	if (!(sended > 0))
	{
		perror("Error en el escritura - socket");
		exit(EXIT_FAILURE);
	}
	return;
}

void recv_mod(int32_t fd, void *buf, size_t n, int32_t flags)
{
	ssize_t recieved = recv(fd, buf, n, flags);
	if (!(recieved > 0))
	{
		perror("Error en la lectura - socket");
		exit(EXIT_FAILURE);
	}
	return;
}

void write_mod(int32_t fd, void *buf, size_t n)
{
	ssize_t written = write(fd, buf, n);
	if (!(written > 0))
	{
		perror("Error en la escritura - pipe");
		exit(EXIT_FAILURE);
	}
	return;
}

void read_mod(int32_t fd, void *buf, size_t n)
{
	ssize_t readed = read(fd, buf, n);
	if (!(readed > 0))
	{
		perror("Error en la lectura - pipe");
		exit(EXIT_FAILURE);
	}
	return;
}

int32_t setUpConnection(struct sockaddr_in *serv_addr, uint16_t port, int32_t max_connections)
{
	int32_t sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror(" apertura de socket ");
		exit(EXIT_FAILURE);
	}
	memset((char *)serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = INADDR_ANY;
	serv_addr->sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *)serv_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("ligadura");
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, max_connections))
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	uint32_t size_server = sizeof(struct sockaddr_in);
	if (getsockname(sockfd, (struct sockaddr *)serv_addr, &size_server) == -1)
	{
		printf("getsockname() failed");
	}
	//printf("port=%d \n", htons(serv_addr->sin_port));
	return sockfd;
}

int32_t acceptConnection(int32_t sockfd, struct sockaddr *cli_addr)
{
	int32_t newsockfd;
	uint32_t clilen = sizeof(struct sockaddr);
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	if (newsockfd < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	return newsockfd;
}

int32_t connect_client(char *address,uint16_t port){
	int32_t sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR apertura de socket");
		exit(EXIT_FAILURE);
	}
	server = gethostbyname(address);
	if (server == NULL)
	{
		fprintf(stderr, "Error, no existe el host\n");
		exit(EXIT_SUCCESS);
	}
	memset((char *)&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (uint32_t)server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("conexion");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}