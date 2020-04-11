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
			uint8_t trys = 0;
			//	TODO = Renombrar ARGUMENT SIZE de  AUTH y SERVER
			STATE state = LOGIN_STATE;
			struct Server_Request *server_request = malloc(sizeof(struct Server_Request));
			struct Server_Response *server_response = malloc(sizeof(struct Server_Response));
			struct Auth_Request *auth_request = malloc(sizeof(struct Auth_Request));
			struct Auth_Response *auth_response = malloc(sizeof(struct Auth_Response));

			memset(auth_request, '0', sizeof(struct Auth_Request));
			memset(auth_response, '0', sizeof(struct Auth_Response));
			char user[ARGUMENT_SIZE] = {0};
			while (1)
			{
				switch (state)
				{
				case LOGIN_STATE:
					if (trys >= MAX_TRY)
					{
						state = EXIT_STATE;
						continue;
					}
					fprintf(stdout, "LOGIN STATE\n");
					recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
					auth_request->code = Auth_LOGIN;
					strncpy(auth_request->first_argument, server_request->first_argument, ARGUMENT_SIZE);
					strncpy(auth_request->second_argument, server_request->second_argument, ARGUMENT_SIZE);
					write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
					read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
					if (auth_response->code == Auth_SUCCESS)
					{
						strncpy(user, server_request->first_argument, ARGUMENT_SIZE);
						server_response->code = Server_LOGIN_SUCCESS;
						strncpy(server_response->first_argument, "LOGIN SUCCESSFUL\n", ARGUMENT_SIZE);
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						state = EXECUTE_STATE;
						continue;
					}
					if (++trys==3)
					{
						server_response->code = Server_LOGIN_REJECTED;
					}else
					{
						server_response->code = Server_LOGIN_FAIL;
					}
					send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
					break;
				case EXECUTE_STATE:
					fprintf(stdout, "EXECUTE STATE\n");
					recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
					fprintf(stdout, "%d\n", server_request->code);
					switch (server_request->code)
					{
					case Server_PASSWD:
						auth_request->code = Auth_PASSWD;
						strncpy(auth_request->first_argument, user, ARGUMENT_SIZE);
						strncpy(auth_request->second_argument, server_request->first_argument, ARGUMENT_SIZE);
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						//fprintf(stdout,"Resultado del cambio de contrasena%s\n%s\n",server_request->first_argument,server_request->second_argument);
						if (auth_response->code == Auth_SUCCESS)
						{
							server_response->code = Server_PASSWD_SUCCESS;
							strncpy(server_response->first_argument, "Password has been changed", ARGUMENT_SIZE);
						}
						else
						{
							server_response->code = Server_PASSWD_FAILED;
							strncpy(server_response->first_argument, "password could not be changed", ARGUMENT_SIZE);
						}
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					case Server_USER_LIST:
						//state = EXIT_STATE;
						break;
					case Server_FILE_LIST:
						/* code */
						break;
					case SERVER_FILE_DOWNLOAD:
						/* code */
						break;
					case SERVER_LOGOUT:
						state = EXIT_STATE;
						server_response->code = Server_LOGOUT_SUCCESS;
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					default:
						break;
					}
					//send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
					break;
				case EXIT_STATE:
					fprintf(stdout,"EXIT STATE\n");
					//close(Auth_fd_1[1]);
					//close(Auth_fd_2[0]);
					close(newsockfd);
					exit(EXIT_SUCCESS);
					break;
				default:
					close(newsockfd);
					exit(EXIT_FAILURE);
					break;
				}
			}
		}
		else
		{
			wait(0);
			//printf("SERVIDOR: Nuevo cliente, que atiende el proceso hijo: %d\n", pid);
			close(newsockfd);
		}
	}

	return 0;
}

//int setUp_comunnication(int fd_1[], int fd_2 [],char * arguments[]);

//gcc -o server transactions.c server_definitions.h auth_definitions.h  server.c