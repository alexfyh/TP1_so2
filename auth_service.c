#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include <signal.h>

#include "auth_functions.h"
#include "transactions.h"

/**
 * @brief Ejecutable que se encarga de recibir peticiones de autorizacion, así como también brindar
 * información de los usuarios y cambio de contraseña 
 * 
 * La base de datos se debe llamar user.csv y debe ser el mismo directorio que el working space
 * del ejecutable auth_service.
 * 
 * Es requerido para su inicio que se pasen los descriptores de archivos de los pipes para
 * la comunicación con otro proceso.
 */

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
        read_mod(fd_read, request, sizeof(struct Auth_Request));
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
            struct UserInfo *userInfo;
            for (rowNumber = 0; rowNumber < usersCount - 1; rowNumber++)
            {
                response->code = Auth_CONTINUE;
                userInfo = getUserInfoByRowNumber(rowNumber);
                if (!userInfo)
                {
                    continue;
                }
                snprintf(response->first_argument, ARGUMENT_SIZE, "%s", userInfo->name);
                snprintf(response->second_argument, ARGUMENT_SIZE, "%s", userInfo->enabled);
                snprintf(response->third_argument, ARGUMENT_SIZE, "%s", userInfo->date);
                write(fd_write, response, sizeof(struct Auth_Response));
            }
            //TODO : ver forma de que la aserción de NULL meterlatmb acá
            response->code = Auth_FINISH;
            userInfo = getUserInfoByRowNumber(rowNumber);
            snprintf(response->first_argument, ARGUMENT_SIZE, "%s", userInfo->name);
            snprintf(response->second_argument, ARGUMENT_SIZE, "%s", userInfo->enabled);
            snprintf(response->third_argument, ARGUMENT_SIZE, "%s", userInfo->date);
            free(userInfo);
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
        write_mod(fd_write, response, sizeof(struct Auth_Response));
    }
}

/*
static void exit_handler(void){
    printf("%d\n",getppid());
    kill (getppid(), 9);
}
*/
