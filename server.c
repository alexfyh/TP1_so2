#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#include "transactions.h"
#include "server_definitions.h"
#include "state.h"
#include "auth_functions.h"
#include "file_functions.h"

#define MAX_CONNECTION 2
#define MAX_TRY 3
#define MD5_LENGTH 16

int main(int argc, char *argv[])
{
	//	INICIO - Comunicacion con Auth_Service
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

	//	INICIO - Comunicacion con File_Service
	int File_fd_1[2];
	int File_fd_2[2];
	if (pipe(File_fd_1) || pipe(File_fd_2))
	{
		perror("File pipe failed");
	}
	pid_t File_pid = fork();
	if (File_pid == -1)
	{
		perror("File Fork Failed");
		exit(EXIT_FAILURE);
	}
	if (!File_pid)
	{
		char str_fd_read[12];
		char str_fd_write[12];
		sprintf(str_fd_read, "%d", File_fd_1[0]);
		sprintf(str_fd_write, "%d", File_fd_2[1]);
		if (execl("file_service", "file_service", str_fd_read, str_fd_write, NULL) == -1)
		{
			perror("Inicio de File_service fallo");
			//TODO = CERRAR TODOS LOS FDs tanto tanto de Auth como File
			exit(EXIT_FAILURE);
		}
	}
	//	FIN - Comunicacion con File_service

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
			getpeername(newsockfd,(struct sockaddr *)&cli_addr,&clilen);
			printf("%s\n",inet_ntoa(cli_addr.sin_addr));
			close(sockfd);
			uint8_t trys = 0;
			//	TODO = Renombrar ARGUMENT SIZE de  AUTH y SERVER
			STATE state = LOGIN_STATE;
			struct Server_Request *server_request = calloc(1,sizeof(struct Server_Request));
			struct Server_Response *server_response = calloc(1,sizeof(struct Server_Response));
			struct Auth_Request *auth_request = calloc(1,sizeof(struct Auth_Request));
			struct Auth_Response *auth_response = calloc(1,sizeof(struct Auth_Response));
			struct File_Request *file_request = calloc(1,sizeof(struct File_Request));
			struct File_Response *file_response = calloc(1,sizeof(struct File_Response));
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
						server_response->responseCode = ServerResponse_LOGIN_SUCCESS;
						strncpy(server_response->first_argument, "LOGIN SUCCESSFUL\n", ARGUMENT_SIZE);
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						state = EXECUTE_STATE;
						continue;
					}
					if (++trys == 3)
					{
						server_response->responseCode = ServerResponse_LOGIN_REJECTED;
					}
					else
					{
						server_response->responseCode = ServerResponse_LOGIN_FAIL;
					}
					send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
					break;
				case EXECUTE_STATE:
					fprintf(stdout, "EXECUTE STATE\n");
					recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
					switch (server_request->requestCode)
					{
					case ServerRequest_PASSWD:
						auth_request->code = Auth_PASSWD;
						strncpy(auth_request->first_argument, user, ARGUMENT_SIZE);
						strncpy(auth_request->second_argument, server_request->first_argument, ARGUMENT_SIZE);
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						//fprintf(stdout,"Resultado del cambio de contrasena%s\n%s\n",server_request->first_argument,server_request->second_argument);
						if (auth_response->code == Auth_SUCCESS)
						{
							server_response->responseCode = ServerResponse_PASSWD_SUCCESS;
						}
						else
						{
							server_response->responseCode = ServerResponse_PASSWD_FAILED;
						}
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					case ServerRequest_USER_LIST:
						auth_request->code = Auth_LIST;
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						while (auth_response->code == Auth_CONTINUE)
						{
							server_response->responseCode = ServerResponse_CONTINUE;
							strncpy(server_response->first_argument, auth_response->first_argument, ARGUMENT_SIZE);
							strncpy(server_response->second_argument, auth_response->second_argument, ARGUMENT_SIZE);
							strncpy(server_response->third_argument, auth_response->third_argument, ARGUMENT_SIZE);
							send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
							read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
							
						}
						server_response->responseCode = ServerResponse_FINISH;
						strncpy(server_response->first_argument, auth_response->first_argument, ARGUMENT_SIZE);
						strncpy(server_response->second_argument, auth_response->second_argument, ARGUMENT_SIZE);
						strncpy(server_response->third_argument, auth_response->third_argument, ARGUMENT_SIZE);
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					case ServerRequest_FILE_LIST:
						file_request->code = File_LIST;
						write_mod(File_fd_1[1], file_request, sizeof(struct File_Request));
						read_mod(File_fd_2[0], file_response, sizeof(struct File_Response));
						while (file_response->code == File_LIST_CONTINUE)
						{
							server_response->responseCode = ServerResponse_FILE_CONTINUE;
							strncpy(server_response->first_argument,file_response->first_argument,ARGUMENT_SIZE);
							strncpy(server_response->second_argument,file_response->second_argument,ARGUMENT_SIZE);
							memcpy(server_response->third_argument,file_response->third_argument,MD5_LENGTH);
							send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
							read_mod(File_fd_2[0], file_response, sizeof(struct File_Response));
						}
						server_response->responseCode = ServerResponse_FILE_FINISH;
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					case ServerRequest_FILE_DOWNLOAD:
						file_request->code = File_DOWNLOAD;
						strncpy(file_request->first_argument,inet_ntoa(cli_addr.sin_addr),ARGUMENT_SIZE);
						strncpy(file_request->second_argument,server_request->first_argument,ARGUMENT_SIZE);
						strncpy(file_request->third_argument,server_request->second_argument,ARGUMENT_SIZE);
						write_mod(File_fd_1[1], file_request, sizeof(struct File_Request));
						//strncpy(file_request->first_argument,server_request->second_argument,ARGUMENT_SIZE);
						//write_mod(File_fd_1[1], file_request, sizeof(struct File_Request));
						break;
					case ServerRequest_LOGOUT:
						state = EXIT_STATE;
						server_response->responseCode = ServerResponse_LOGOUT;
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					default:
						break;
					}
					//send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
					break;
				case EXIT_STATE:
					fprintf(stdout, "EXIT STATE\n");
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