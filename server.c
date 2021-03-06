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

#define MAX_CONNECTION 1
#define MAX_TRY 3
#define MD5_LENGTH 16
#define FD_STRING_LENGTH 12
static const char *file_service = "file_service";
static const char *auth_service = "auth_service";

void createChildProccess(int[], int[], const char *);

/**
 * @brief Servidor principal, que crea los servicios de autorización y archivos.
 * Se maneja por una máquina de estados
 * 
 * Primer argumento = nro de puerto
 */
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Uso: %s <puerto>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int32_t Auth_fd_1[2];
	int32_t Auth_fd_2[2];
	createChildProccess(Auth_fd_1, Auth_fd_2, auth_service);

	int32_t File_fd_1[2];
	int32_t File_fd_2[2];
	createChildProccess(File_fd_1, File_fd_2, file_service);

	struct sockaddr_in serv_addr, cli_addr;
	int32_t sockfd = setUpConnection(&serv_addr, argv[1], MAX_CONNECTION);
	uint32_t clilen;
	clilen = sizeof(cli_addr);
	while (1)
	{
		int32_t newsockfd = acceptConnection(sockfd, (struct sockaddr *)&cli_addr);
		int32_t pid = fork();
		if (pid < 0)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if (pid == 0)
		{
			getpeername(newsockfd, (struct sockaddr *)&cli_addr, &clilen);
			close(sockfd);
			uint8_t trys = 0;
			//	TODO = Renombrar ARGUMENT SIZE de  AUTH y SERVER
			STATE state = LOGIN_STATE;
			struct Server_Request *server_request = (struct Server_Request *)calloc(1, sizeof(struct Server_Request));
			struct Server_Response *server_response = (struct Server_Response *)calloc(1, sizeof(struct Server_Response));
			struct Auth_Request *auth_request = (struct Auth_Request *)calloc(1, sizeof(struct Auth_Request));
			struct Auth_Response *auth_response = (struct Auth_Response *)calloc(1, sizeof(struct Auth_Response));
			struct File_Request *file_request = (struct File_Request *)calloc(1, sizeof(struct File_Request));
			struct File_Response *file_response = (struct File_Response *)calloc(1, sizeof(struct File_Response));
			char user[ARGUMENT_SIZE] = {0};
			while (1)
			{
				switch (state)
				{
				case LOGIN_STATE:
				{
					if (trys >= MAX_TRY)
					{
						state = EXIT_STATE;
						continue;
					}
					fprintf(stdout, "LOGIN STATE\n");
					recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
					if (server_request->requestCode==ServerRequest_LOGOUT)
					{
						state=EXIT_STATE;
						continue;
					}
					
					auth_request->code = Auth_LOGIN;
					snprintf(auth_request->first_argument, ARGUMENT_SIZE, "%s", server_request->first_argument);
					snprintf(auth_request->second_argument, ARGUMENT_SIZE, "%s", server_request->second_argument);
					write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
					read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
					if (auth_response->code == Auth_SUCCESS)
					{
						server_response->responseCode = ServerResponse_LOGIN_SUCCESS;
						snprintf(user, ARGUMENT_SIZE, "%s", server_request->first_argument);
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
				}
				case EXECUTE_STATE:
				{
					fprintf(stdout, "EXECUTE STATE\n");
					recv_mod(newsockfd, server_request, sizeof(struct Server_Request), 0);
					switch (server_request->requestCode)
					{
					case ServerRequest_PASSWD:
					{
						auth_request->code = Auth_PASSWD;
						snprintf(auth_request->first_argument, ARGUMENT_SIZE, "%s", user);
						snprintf(auth_request->second_argument, ARGUMENT_SIZE, "%s", server_request->first_argument);
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
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
					}
					case ServerRequest_USER_LIST:
					{
						auth_request->code = Auth_LIST;
						write_mod(Auth_fd_1[1], auth_request, sizeof(struct Auth_Request));
						read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						while (auth_response->code == Auth_CONTINUE)
						{
							server_response->responseCode = ServerResponse_CONTINUE;
							snprintf(server_response->first_argument, ARGUMENT_SIZE, "%s", auth_response->first_argument);
							snprintf(server_response->second_argument, ARGUMENT_SIZE, "%s", auth_response->second_argument);
							snprintf(server_response->third_argument, ARGUMENT_SIZE, "%s", auth_response->third_argument);
							send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
							read_mod(Auth_fd_2[0], auth_response, sizeof(struct Auth_Response));
						}
						server_response->responseCode = ServerResponse_FINISH;
						snprintf(server_response->first_argument, ARGUMENT_SIZE, "%s", auth_response->first_argument);
						snprintf(server_response->second_argument, ARGUMENT_SIZE, "%s", auth_response->second_argument);
						snprintf(server_response->third_argument, ARGUMENT_SIZE, "%s", auth_response->third_argument);
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					}
					case ServerRequest_FILE_LIST:
					{
						file_request->code = File_LIST;
						write_mod(File_fd_1[1], file_request, sizeof(struct File_Request));
						read_mod(File_fd_2[0], file_response, sizeof(struct File_Response));
						while (file_response->code == File_LIST_CONTINUE)
						{
							server_response->responseCode = ServerResponse_FILE_CONTINUE;
							snprintf(server_response->first_argument, ARGUMENT_SIZE, "%s", file_response->first_argument);
							snprintf(server_response->second_argument, ARGUMENT_SIZE, "%s", file_response->second_argument);
							memcpy(server_response->third_argument, file_response->third_argument, MD5_LENGTH);
							send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
							read_mod(File_fd_2[0], file_response, sizeof(struct File_Response));
						}
						server_response->responseCode = ServerResponse_FILE_FINISH;
						send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					}
					case ServerRequest_FILE_DOWNLOAD:
					{
						file_request->code = File_DOWNLOAD;
						snprintf(file_request->first_argument, ARGUMENT_SIZE, "%s", inet_ntoa(cli_addr.sin_addr));
						snprintf(file_request->second_argument, ARGUMENT_SIZE, "%s", server_request->first_argument);
						snprintf(file_request->third_argument, ARGUMENT_SIZE, "%s", server_request->second_argument);
						write_mod(File_fd_1[1], file_request, sizeof(struct File_Request));
						break;
					}
					case ServerRequest_LOGOUT:
					{
						state = EXIT_STATE;
						//server_response->responseCode = ServerResponse_LOGOUT;
						//send_mod(newsockfd, server_response, sizeof(struct Server_Response), 0);
						break;
					}
					default:
						break;
					}
					break;
				}
				case EXIT_STATE:
				{
					fprintf(stdout, "EXIT STATE\n");
					close(newsockfd);
					exit(EXIT_SUCCESS);
				}
				}
			}
		}
		else
		{
			wait(NULL);
			//Si la conexión con el cliente finalizó y no llego a elstado exit.???
			close(newsockfd);
		}
	}
}

/**
 * @brief Crea un proceso hijo, le asigna  correr un ejectuble y le pasa descriptores
 * de archivo para comunicarse con él
 *
 * @param file_descriptor_1 fd_1[0] para que proceso hijo escuche,fd_1[1] para que proceso padre escriba
 * @param file_descriptor_2 fd_2[0] para que proceso hijo escriba,fd_2[1] para que proceso padre escriba.
 * @param executable Nombre del ejecutable,ubicado en el mismo directorio de trabajo.
 */
void createChildProccess(int file_descriptor_1[2], int file_descriptor_2[2], const char *executable)
{
	if (pipe(file_descriptor_1) || pipe(file_descriptor_2))
	{
		perror("Pipe failed");
		exit(EXIT_FAILURE);
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("Fork Failed");
		exit(EXIT_FAILURE);
	}
	if (!pid)
	{
		char str_fd_read[FD_STRING_LENGTH];
		char str_fd_write[FD_STRING_LENGTH];
		snprintf(str_fd_read, FD_STRING_LENGTH, "%d", file_descriptor_1[0]);
		snprintf(str_fd_write, FD_STRING_LENGTH, "%d", file_descriptor_2[1]);
		if (execl(executable, executable, str_fd_read, str_fd_write, NULL) == -1)
		{
			perror("Inicio del servicio  auxiliar fallo");
			exit(EXIT_FAILURE);
		}
	}
	return;
}
