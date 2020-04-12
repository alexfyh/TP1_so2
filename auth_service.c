#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include "auth_functions.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("File descriptors needed!!!\n");
        //perror("File descriptors needed!!!");
        exit(EXIT_FAILURE);
    }
    int32_t fd_read = atoi(argv[1]);
    int32_t fd_write = atoi(argv[2]);
    if (!fd_read || !fd_write) 
    {
        perror("Casteo de descriptores fallido");
        //  TODO = debería avisar al ppid que fallo y finalizarlo
        exit(EXIT_FAILURE);
    }
    
    struct Auth_Request *request = malloc(sizeof(struct Auth_Request));
    struct Auth_Response *response =malloc(sizeof(struct Auth_Response));
    //  TODO = debería alocar memoria??? //memory gone wild
    //memset(&response,0,sizeof(struct Auth_Response));
    ssize_t n;

    while (1)
    {
        n = read(fd_read, request, sizeof(struct Auth_Request));
        if (n != sizeof(struct Auth_Request))
        {
            perror("No se ha leído la estructura correctamente");
            response->code = Auth_FAIL;
        }
        else
        {
            uint32_t usersCount;

            switch (request->code)
            {
            case Auth_LOGIN:
                if (isAuthorized(request->first_argument, request->second_argument))
                {
                    response->code = Auth_SUCCESS;
                }
                else
                {
                    response->code = Auth_FAIL;
                }
                break;
            case Auth_LIST:
                usersCount = getUsersCount();
                if (usersCount == 0)
                {
                    //Si se borraron los usuarios justo,sale del switch y envia error
                    response->code = Auth_LIST_FAIL;
                    break;
                }
                uint32_t rowNumber = 0;
                struct UserInfo userInfo;
                for (rowNumber = 0; rowNumber < usersCount-1; rowNumber++)
                {
                    userInfo = getUserInfoByRowNumber(rowNumber);
                    strncpy(response->first_argument,userInfo.name,ARGUMENT_SIZE);
                    strncpy(response->second_argument,userInfo.enabled,ARGUMENT_SIZE);
                    strncpy(response->third_argument,userInfo.date,ARGUMENT_SIZE);
                    response->code = Auth_CONTINUE;
                    write(fd_write, response, sizeof(struct Auth_Response));           
                }
                userInfo = getUserInfoByRowNumber(rowNumber);
                strncpy(response->first_argument,userInfo.name,ARGUMENT_SIZE);
                strncpy(response->second_argument,userInfo.enabled,ARGUMENT_SIZE);
                strncpy(response->third_argument,userInfo.date,ARGUMENT_SIZE);
                response->code = Auth_FINISH;            
                break;
            case Auth_PASSWD:
                if(setUserPassword(request->first_argument,request->second_argument)){
                    response->code = Auth_SUCCESS;
                }
                else
                {
                    response->code = Auth_FAIL;
                }
                break;
            default:
                response->code = Auth_FAIL;
                break;
            }
        }
        n = write(fd_write, response, sizeof(struct Auth_Response));
        if (n!=sizeof(struct Auth_Response))
        {
            //  TODO = cómo se debería comportar teniendo en cuenta que
            //  del otro lado se quedó colgado escuchando ???
            perror("No se ha escrito  correctamente la respuesta");
        }    
    }
    //  Cómo llegaste acá???
    exit(EXIT_SUCCESS);
}

//gcc -o  auth_service auth_definitions.h auth_functions.c auth_service.c