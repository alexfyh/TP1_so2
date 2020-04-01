#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "buffer_functions.h"
#include "login_functions.h"

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	uint16_t puerto;
	char buffer[BUFFER_SIZE];
	memset(buffer,'\0',sizeof(buffer));
	if (argc < 3)
	{
		fprintf(stderr, "Uso %s host puerto\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	puerto = (uint16_t)((unsigned int)atoi(argv[2]));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR apertura de socket");
		exit(EXIT_FAILURE);
	}

	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr, "Error, no existe el host\n");
		exit(EXIT_SUCCESS);
	}
	memset((char *)&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (uint32_t)server->h_length);
	serv_addr.sin_port = htons(puerto);
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("conexion");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		//	TODO = ponele voluntad, la ctm
		//	TODO = ver de usar la versión safe de printf
		printf("%s", read_buffer(sockfd, buffer));
		if (!strncmp(buffer, LOGIN_FAIL, BUFFER_SIZE))
		{
			exit(EXIT_SUCCESS);
		}
		write_buffer(sockfd, NULL, buffer);
	}
	return 0;
}