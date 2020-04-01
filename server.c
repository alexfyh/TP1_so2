#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include "buffer_functions.h"
#include "login_functions.h"
#include "state.c"

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
		exit(EXIT_FAILURE);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror(" apertura de socket ");
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}
	printf("Proceso: %d - socket disponible: %d\n", getpid(), ntohs(serv_addr.sin_port));
	if (listen(sockfd, MAX_CONNECTION))
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	clilen = sizeof(cli_addr);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid == 0)
		{
			close(sockfd);
			uint8_t trys = MAX_TRY;
			char user_input[BUFFER_SIZE];
			STATE state = LOGIN_STATE;

			//	TODO = Ver si es necesario guardar la contrasena
			char password_input[BUFFER_SIZE];
			while (1)
			{
				switch (state)
				{
				case LOGIN_STATE:
					write_buffer(newsockfd, LOGIN, buffer);
					read_buffer(newsockfd, buffer);
					strncpy(user_input, buffer, BUFFER_SIZE - 1);
					write_buffer(newsockfd, PASSWORD, buffer);
					read_buffer(newsockfd, buffer);
					strncpy(password_input, buffer, BUFFER_SIZE - 1);
					printf("Usuario: %s \nPassword: %s\n", user_input, password_input);
					bool login_result = isAuthorized(user_input, password_input);

					if (login_result)
					{
						state = EXECUTE_STATE;
						strncpy(buffer, LOGIN_SUCCESS, strlen(LOGIN_SUCCESS));
						write_buffer(newsockfd, strncpy(buffer, user_input, BUFFER_SIZE), buffer);
					}
					else
					{
						trys--;
						if (!trys)
						{
							state = DENIED_STATE;
							write_buffer(newsockfd, LOGIN_FAIL, buffer);
						}
					}
					break;
				case EXECUTE_STATE:
					while (1)
					{
						printf("Ejecutando comando %s", read_buffer(newsockfd, buffer));
						write_buffer(newsockfd, user_input, buffer);
					}
					break;

				case DENIED_STATE:
					exit(EXIT_SUCCESS);
					break;

				default:
					exit(EXIT_FAILURE);
					break;
				}
			}
			//	Sesión finalizada
			printf("Esto es todo, amigos. \nFinalizado proceso hijo.Salu2\n");
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid);
			close(newsockfd);
		}
	}
	return 0;
}
