#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include <signal.h>

#include "auth_functions.h"

//static void exit_handler(void);

int main(int argc, char *argv[])
{
    //atexit(exit_handler);
    if (argc != 3)
    {
        printf("File descriptors needed!!!\n");
        exit(EXIT_FAILURE);
    }
    int32_t fd_read = atoi(argv[1]);
    int32_t fd_write = atoi(argv[2]);
    if (!fd_read || !fd_write)
    {
        perror("Casteo de descriptores fallido");
        exit(EXIT_FAILURE);
    }
    struct Auth_Request *request = (struct Auth_Request *)calloc(1, sizeof(struct Auth_Request));
    struct Auth_Response *response = (struct Auth_Response *)calloc(1, sizeof(struct Auth_Response));

    while (1)
    {
        ssize_t n;
        n = read(fd_read, request, sizeof(struct Auth_Request));
        if (n != sizeof(struct Auth_Request))
        {
            perror("No se ha leído la estructura correctamente");
            response->code = Auth_FAIL;
        }
        else
        {
            switch (request->code)
            {
            case Auth_LOGIN:
            {
                if (isAuthorized(request->first_argument, request->second_argument))
                {
                    response->code = Auth_SUCCESS;
                }
                else
                {
                    response->code = Auth_FAIL;
                }
                break;
            }
            case Auth_LIST:
            {
                uint32_t usersCount = getUsersCount();
                if (usersCount == 0)
                {
                    //Si se borraron los usuarios justo,sale del switch y envia error
                    response->code = Auth_LIST_FAIL;
                    break;
                }
                uint32_t rowNumber = 0;
                struct UserInfo userInfo;
                for (rowNumber = 0; rowNumber < usersCount - 1; rowNumber++)
                {
                    userInfo = getUserInfoByRowNumber(rowNumber);
                    strncpy(response->first_argument, userInfo.name, ARGUMENT_SIZE);
                    strncpy(response->second_argument, userInfo.enabled, ARGUMENT_SIZE);
                    strncpy(response->third_argument, userInfo.date, ARGUMENT_SIZE);
                    response->code = Auth_CONTINUE;
                    write(fd_write, response, sizeof(struct Auth_Response));
                }
                userInfo = getUserInfoByRowNumber(rowNumber);
                strncpy(response->first_argument, userInfo.name, ARGUMENT_SIZE);
                strncpy(response->second_argument, userInfo.enabled, ARGUMENT_SIZE);
                strncpy(response->third_argument, userInfo.date, ARGUMENT_SIZE);
                response->code = Auth_FINISH;
                break;
            }
            case Auth_PASSWD:
            {
                if (setUserPassword(request->first_argument, request->second_argument))
                {
                    response->code = Auth_SUCCESS;
                }
                else
                {
                    response->code = Auth_FAIL;
                }
                break;
            }
            }
        }
        n = write(fd_write, response, sizeof(struct Auth_Response));
        if (n != sizeof(struct Auth_Response))
        {
            //  TODO = cómo se debería comportar teniendo en cuenta que
            //  del otro lado se quedó colgado escuchando ???
            perror("No se ha escrito  correctamente la respuesta");
        }
    }
}

/*
static void exit_handler(void){
    printf("%d\n",getppid());
    kill (getppid(), 9);
}
*/
