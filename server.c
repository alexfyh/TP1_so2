#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define TAM 256
#define MAX_CONNECTION 5
#define MAX_TRY 3

int main(int argc, char *argv[])
{
	// MAX PID en 64-bit 2^22
	int32_t sockfd, newsockfd, pid;
	uint32_t clilen;
	uint16_t puerto;
	char buffer[TAM];
	struct sockaddr_in serv_addr, cli_addr;
	ssize_t n;

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
				char user_input[TAM];
				char password_input[TAM];
				{
					memset(buffer, '\0', TAM);
					n = write(newsockfd, "login:", TAM - 1);
					if (n < 0)
					{
						perror("escritura de socket");
						exit(1);
					}
					n = read(newsockfd, buffer, TAM - 1);
					if (n < 0)
					{
						perror("lectura de socket");
						exit(1);
					}
					strncpy(user_input, buffer, TAM - 1);
				}

				{
					memset(buffer, '\0', TAM);
					n = write(newsockfd, "password:", strnlen("password:", TAM - 1));
					if (n < 0)
					{
						perror("escritura de socket");
						exit(1);
					}
					n = read(newsockfd, buffer, TAM - 1);
					if (n < 0)
					{
						perror("lectura de socket");
						exit(1);
					}
					strncpy(password_input, buffer, TAM - 1);
				}

				printf("Usuario: %s \nPassword: %s", user_input, password_input);
				exit(0);

				/*
				memset(buffer, 0, TAM);
				n = read(newsockfd, buffer, TAM - 1);
				if (n < 0)
				{
					perror("lectura de socket");
					exit(1);
				}
				printf("PROCESO %d. ", getpid());
				printf("Recibí: %s", buffer);
				n = write(newsockfd, "Obtuve su mensaje", 18);
				if (n < 0)
				{
					perror("escritura en socket");
					exit(1);
				}
				// Verificación de si hay que terminar
				buffer[strlen(buffer) - 1] = '\0';
				if (!strcmp("fin", buffer))
				{
					printf("PROCESO %d. Como recibí 'fin', termino la ejecución.\n\n", getpid());
					exit(0);
				}
				*/
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