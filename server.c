#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "transactions.h"
#include "server_definitions.h"
#include "state.h"

#include "auth_definitions.h"

#define MAX_CONNECTION 2
#define MAX_TRY 3

int main(int argc, char *argv[])
{
	//	Primero definir los pipe de comunicación con Server Service y File Service
	//	INICIO - Comunicacion con Server_service
	int Auth_fd_1[2];
	int Auth_fd_2[2];
	if (pipe(Auth_fd_1) || pipe(Auth_fd_2))
	{
		perror("Authorization pipe failed");
	}
	pid_t Auth_pid = fork();
	if (Auth_pid == -1)
	{
		perror("Authorization Fork Failed");
		exit(EXIT_FAILURE);
	}
	if (!Auth_pid)
	{
		char str_fd_read[12];
		char str_fd_write[12];
		sprintf(str_fd_read, "%d", Auth_fd_1[0]);
		sprintf(str_fd_write, "%d", Auth_fd_2[1]);
		if (execl("auth_service", "auth_service", str_fd_read, str_fd_write, NULL) == -1)
		{
			perror("Inicio de Auth_service fallo");
			//TODO = CERRAR TODOS LOS FDs tanto tanto de Auth como File
			exit(EXIT_FAILURE);
		}
	}
	//	FIN - Comunicacion con Auth_service

	/*
	ssize_t n = write(Auth_fd_1[1], &request, sizeof(struct Server_Request));
	if (n < 0)
	{
		perror("Fallo en escritura");
	}
	else
	{
		n = read(Auth_fd_2[0], response, sizeof(struct Server_Response));
		printf("Respuesta :%d   %s\n", response->code, response->first_argument);
		wait(NULL);
	}
*/

	// MAX PID en 64-bit 2^22
	int32_t sockfd, newsockfd, pid;
	uint32_t clilen;
	uint16_t puerto;
	struct sockaddr_in serv_addr, cli_addr;

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
			//	TODO = Renombrar ARGUMENT SIZE de  AUTH y SERVER 
			STATE state = LOGIN_STATE;
			struct Server_Request *server_request = malloc(sizeof(struct Server_Request));
			struct Server_Response *server_response = malloc(sizeof(struct Server_Response));
			struct Auth_Request *auth_request = malloc(sizeof(struct Auth_Request));
			struct Auth_Response *auth_response = malloc(sizeof(struct Auth_Response));
			while (1)
			{
				switch (state)
				{
				case LOGIN_STATE:
					do
					{
						printf("Size =%lu\n",sizeof(struct Server_Request));
						recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
						//	TODO = Ver si el cliente me manda diferente
						strncpy(auth_request->first_argument,server_request->first_argument,ARGUMENT_SIZE);
						strncpy(auth_request->second_argument,server_request->second_argument,ARGUMENT_SIZE);
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						if(auth_response->code==Auth_SUCCESS){
							server_response->code=Server_LOGIN_SUCCESS;
							state=EXECUTE_STATE;
						}
						else if (trys>1)
						{
							server_response->code=Server_LOGIN_FAIL;
						}else
						{
							server_response->code=Server_LOGIN_REJECTED;
						}
						send_mod(newsockfd,server_response,sizeof(struct Server_Response),0);

					} while (--trys);
					state = EXIT_STATE;

					/*
					write_buffer(newsockfd, LOGIN, buffer);
					read_buffer(newsockfd, buffer);
					strncpy(user_input, buffer, BUFFER_SIZE - 1);
					write_buffer(newsockfd, PASSWORD, buffer);
					read_buffer(newsockfd, buffer);
					strncpy(password_input, buffer, BUFFER_SIZE - 1);
					printf("Usuario: %s \nPassword: %s\n", user_input, password_input);
					bool login_result = isServerorized(user_input, password_input);

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
					*/
					break;
				case EXECUTE_STATE:
					while (1)
					{
						//printf("Ejecutando comando %s", read_buffer(newsockfd, buffer));
						//write_buffer(newsockfd, user_input, buffer);
					}
					break;

				case EXIT_STATE:
					exit(EXIT_SUCCESS);
					break;

				default:
					exit(EXIT_FAILURE);
					break;
				}
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

//int setUp_comunnication(int fd_1[], int fd_2 [],char * arguments[]);


//gcc -o server transactions.c server_definitions.h auth_definitions.h  server.c 