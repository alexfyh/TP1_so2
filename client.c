#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "transactions.h" 
#include "server_definitions.h"
#include "state.h"

#define BUFFER_SIZE 120

int main(int argc, char *argv[])
{
	int32_t sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	uint16_t puerto;
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
	struct Server_Request *request = malloc(sizeof(struct Server_Request));
	struct Server_Response *response = malloc(sizeof(struct Server_Response));
	char user [ARGUMENT_SIZE]="";
	char buffer[BUFFER_SIZE]="";
	STATE state=LOGIN_STATE;
	int32_t send_flags = 0;
	int32_t recv_flags = 0;
	
	while (1)
	{
		switch (state)
		{
		case LOGIN_STATE:
			printf("login:");
			fgets(user,ARGUMENT_SIZE,stdin);
			user[strcspn(user, "\n")] = 0;
			printf("password:");
			fgets(buffer,ARGUMENT_SIZE,stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			strncpy(request->first_argument,user,ARGUMENT_SIZE);
			strncpy(request->second_argument,buffer,ARGUMENT_SIZE);
			request->code=Server_LOGIN;
			send_mod(sockfd,request,sizeof(struct Server_Request),send_flags);
			recv_mod(sockfd,response,sizeof(struct Server_Response),recv_flags);
			if (response->code==Server_LOGIN_SUCCESS)
			{
				printf("\nSuccessful login\n");
				state=EXECUTE_STATE;	
			}
			else if (response->code==Server_LOGIN_REJECTED)
			{
				printf("\nSession rejected\n");
				state=EXIT_STATE;
			}	
			break;
		case EXECUTE_STATE:
			printf("%s$",user);
			
			exit(0);
			
			break;
		case EXIT_STATE:
			printf("\nSession completed\n");
			close(sockfd);
			exit(EXIT_SUCCESS);
			break;
		default:
			perror("Estado no determinado...????");
			printf("\nSession completed\n");
			close(sockfd);
			exit(EXIT_FAILURE);
			break;
		}
	}
	return 0;
}


//gcc -o client server_definitions.h state.h  transactions.c client.c