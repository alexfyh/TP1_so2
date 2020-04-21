#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include "transactions.h"
#include "server_definitions.h"
#include "client_functions.h"
#include "state.h"
#include "file_functions.h"

#define BUFFER_SIZE 120
#define FILE_BUFFER_SIZE 500

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Uso %s host puerto\n", argv[0]);
		exit(EXIT_SUCCESS);
	}
	int32_t sockfd = connectToServer(argv[1], argv[2]);
	struct Server_Request *request = (struct Server_Request *)calloc(1, sizeof(struct Server_Request));
	struct Server_Response *response = (struct Server_Response *)calloc(1, sizeof(struct Server_Response));
	char user[ARGUMENT_SIZE] = "";
	char buffer[BUFFER_SIZE] = "";
	STATE state = LOGIN_STATE;
	int32_t send_flags = 0;
	int32_t recv_flags = 0;

	while (1)
	{
		switch (state)
		{
		case LOGIN_STATE:
		{
			printf("login:");
			buffer[strcspn(fgets(buffer, ARGUMENT_SIZE, stdin), "\n")] = 0;
			snprintf(user, ARGUMENT_SIZE, "%s", buffer);
			printf("password:");
			buffer[strcspn(fgets(buffer, ARGUMENT_SIZE, stdin), "\n")] = 0;
			snprintf(request->first_argument, ARGUMENT_SIZE, "%s", user);
			snprintf(request->second_argument, ARGUMENT_SIZE, "%s", buffer);
			request->requestCode = ServerRequest_LOGIN;
			send_mod(sockfd, request, sizeof(struct Server_Request), send_flags);
			recv_mod(sockfd, response, sizeof(struct Server_Response), recv_flags);
			if (response->responseCode == ServerResponse_LOGIN_SUCCESS)
			{
				printResponse(response);
				state = EXECUTE_STATE;
			}
			else if (response->responseCode == ServerResponse_LOGIN_REJECTED)
			{
				printResponse(response);
				state = EXIT_STATE;
			}
			break;
		}
		case EXECUTE_STATE:
		{
			printf("%s$", user);
			buffer[strcspn(fgets(buffer, BUFFER_SIZE, stdin), "\n")] = 0;
			char *image_name = (char *) calloc(1,ARGUMENT_SIZE);
			if (formatRequest(buffer, sizeof(buffer), request, image_name))
			{
				if (request->requestCode == ServerRequest_FILE_DOWNLOAD)
				{
					int32_t fd_download = open(image_name, O_RDWR | O_CREAT,0666);
					if (fd_download <0)
					{
						perror("No se pudo crear el descriptor del archivo de destino");
						continue;
					}
					struct sockaddr_in serv_addr, cli_addr;
					//TODO = modificar la salida para que no termine abruptamente sino CONTINUE
					int32_t file_sockfd = setUpConnection(&serv_addr, NULL, 1);
					snprintf(request->first_argument, ARGUMENT_SIZE, "%d", (uint16_t)htons(serv_addr.sin_port));
					send_mod(sockfd, request, sizeof(struct Server_Request), send_flags);
					int32_t file_newsockfd = acceptConnection(file_sockfd, (struct sockaddr *)&cli_addr);
					char file_buffer[FILE_BUFFER_SIZE] = {0};
					ssize_t bytes_recv;
					do
					{
						bytes_recv = recv(file_newsockfd, file_buffer, sizeof(file_buffer), 0);
						if (bytes_recv < 0)
						{
							perror("Error en descarga");
						}
						write(fd_download, file_buffer, (size_t)bytes_recv);
					} while (bytes_recv > 0);
					struct _MBR MBR;
					lseek(fd_download,0,SEEK_SET);
					read(fd_download, &MBR, sizeof(struct _MBR));
					printPartitionTable(&MBR);

					close(fd_download);
					close(file_sockfd);
				}
				else
				{
					send_mod(sockfd, request, sizeof(struct Server_Request), send_flags);
					do
					{
						recv_mod(sockfd, response, sizeof(struct Server_Response), recv_flags);
						printResponse(response);
					} while (response->responseCode == ServerResponse_CONTINUE || response->responseCode == ServerResponse_FILE_CONTINUE);
					if (response->responseCode == ServerResponse_LOGOUT)
					{
						state = EXIT_STATE;
					}
				}
			}
			else if (strncmp(buffer, "", 1))
			{
				printf("Command not found\n");
			}
			break;
		}
		case EXIT_STATE:
		{
			close(sockfd);
			free(request);
			free(response);
			exit(EXIT_SUCCESS);
		}
		}
	}
}

//https://gist.github.com/aspyct/3462238
//https://www.geeksforgeeks.org/signals-c-language/
