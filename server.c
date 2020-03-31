#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "buffer_functions.h"

#define MAX_CONNECTION 5
#define MAX_TRY 3

int main(int argc, char *argv[])
{
	// MAX PID en 64-bit 2^22
	int32_t sockfd, newsockfd, pid;
	uint32_t clilen;
	uint16_t puerto;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in serv_addr, cli_addr;
	//size_t n;

	if (argc < 2)
	{
		fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror(" apertura de socket ");
		exit(1);
	}
	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	//	TODO = error si el resultado de atoi es menor a 0 o 1024.
	//	Ver si se puede con strtol() con ERANGE definido (https://gist.github.com/deltheil/7502883)
	puerto = (uint16_t)((unsigned int)atoi(argv[1]));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(puerto);
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ligadura");
		exit(1);
	}
	printf("Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port));
	if (listen(sockfd, MAX_CONNECTION))
	{
		perror("listen");
		exit(1);
	}
	clilen = sizeof(cli_addr);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		{
			perror("accept");
			exit(1);
		}

		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(1);
		}

		if (pid == 0)
		{
			close(sockfd);
			while (1)
			{
				char user_input[BUFFER_SIZE];
				//	TODO = Ver si es necesario guardar la contrasena
				char password_input[BUFFER_SIZE];
				
				write_buffer(newsockfd,LOGIN,buffer);
				read_buffer(newsockfd,buffer);
				strncpy(user_input, buffer, BUFFER_SIZE - 1);

				write_buffer(newsockfd,PASSWORD,buffer);
				read_buffer(newsockfd,buffer);
				strncpy(password_input, buffer, BUFFER_SIZE - 1);

				printf("Usuario: %s \nPassword: %s", user_input, password_input);
				printf("Esto es todo, amigos. \nFinalizado proceso hijo.Salu2");
				exit(0);
			}
		}
		else
		{
			printf("SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid);
			close(newsockfd);
		}
	}
	return 0;
}


