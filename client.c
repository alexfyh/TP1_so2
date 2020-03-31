#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "buffer_functions.h"
#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	//int terminar = 0;

	uint16_t puerto;

	char buffer[BUFFER_SIZE];
	if (argc < 3)
	{
		fprintf(stderr, "Uso %s host puerto\n", argv[0]);
		exit(0);
	}
	puerto = (uint16_t)((unsigned int)atoi(argv[2]));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR apertura de socket");
		exit(1);
	}

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "Error, no existe el host\n");
		exit(0);
	}
	memset((char *)&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (uint32_t)server->h_length);
	serv_addr.sin_port = htons(puerto);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("conexion");
		exit(1);
	}

	while (1)
	{
		read_buffer(sockfd, buffer);
		//	TODO = ponele voluntad, la ctm
		printf("%s",buffer);
		write_buffer(sockfd, NULL, buffer);
		
		//strncpy(user_input, buffer, BUFFER_SIZE - 1);
		read_buffer(sockfd, buffer);
		printf("%s",buffer);
		write_buffer(sockfd, NULL, buffer);
		//strncpy(password_input, buffer, BUFFER_SIZE - 1);
		exit(0);
	}
	return 0;
}