//TODO = Ver si es necesario definirlo a fuera del client.c

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>

#include "server_definitions.h"
#include "file_functions.h"

static const char *EXIT_CMD = "exit";
static const char *LS_CMD = "ls";
static const char *USER_CMD = "user";
static const char *PASSWD_CMD = "passwd";
static const char *FILE_CMD = "file";
static const char *DOWN_CMD = "down";

uint8_t getArgumentsCount(char *, uint32_t);
bool formatRequest(char *, uint32_t, struct Server_Request *, char *);
void printResponse(struct Server_Response *);

/**
 * @brief Devuelve la cantidad de argumentos leídos por consola,
 * separados por espacios en blanco
 * 
 * @param buffer String a parsear
 * @param buffer_size Tamaño del buffer de entrada
 * @return uint8_t Cantidad de argumentos
 */
uint8_t getArgumentsCount(char *buffer, uint32_t buffer_size)
{
    char *temp_buffer = (char *)calloc(1, buffer_size);
    snprintf(temp_buffer, buffer_size, "%s", buffer);
    uint8_t count = 0;
    char *token = strtok(temp_buffer, " ");
    while (token)
    {
        count++;
        token = strtok(NULL, " ");
    }
    free(temp_buffer);
    return count;
}

/**
 * @brief Confirma si lo ingresado corresponde un conjunto de comandos y argumentos
 * válidos. En caso afirmativo, modifica la estructura pasado por referencia para
 *  armar la petción.
 * 
 * @param buffer Buffer correspondiente a la entrada ingresada por el usuario.
 * @param buffer_size Tamaño del buffer
 * @param request Estructura en la que se arma la petición
 * @param image_name Nombre de la imagen a descargar
 * @return true Lo ingresado corresponde a una petición válida
 * @return false 
 */
bool formatRequest(char *buffer, uint32_t buffer_size, struct Server_Request *request, char *image_name)
{
    bool result = false;
    char *temp_buffer = (char *)calloc(1, buffer_size);
    snprintf(temp_buffer, buffer_size, "%s", buffer);
    char *token = strtok_r(temp_buffer, " ", &temp_buffer);
    if (!token)
    {
        return false;
    }
    //printf("Primer argumento= %s\n", token);
    uint8_t cant_argumentos = getArgumentsCount(buffer, buffer_size);
    if (cant_argumentos == 1 && !strncmp(token, EXIT_CMD, ARGUMENT_SIZE))
    {
        //printf("Exit command\n");
        request->requestCode = ServerRequest_LOGOUT;
        result = true;
    }
    else if (!strncmp(token, USER_CMD, ARGUMENT_SIZE))
    {
        token = strtok_r(temp_buffer, " ", &temp_buffer);
        //printf("Segundo argumento= %s\n", token);
        if (cant_argumentos == 2 && !strncmp(token, LS_CMD, ARGUMENT_SIZE))
        {
            //printf("USER LIST\n");
            request->requestCode = ServerRequest_USER_LIST;
            result = true;
        }
        else if (cant_argumentos == 3 && !strncmp(token, PASSWD_CMD, ARGUMENT_SIZE))
        {
            token = strtok_r(temp_buffer, " ", &temp_buffer);
            //printf("Tercer argumento= %s\n", token);
            request->requestCode = ServerRequest_PASSWD;
            snprintf(request->first_argument, ARGUMENT_SIZE, "%s", token);
            result = true;
        }
    }
    else if (!strncmp(token, FILE_CMD, ARGUMENT_SIZE))
    {
        token = strtok_r(temp_buffer, " ", &temp_buffer);
        //printf("Segundo argumento= %s\n", token);
        if (cant_argumentos == 2 && !strncmp(token, LS_CMD, ARGUMENT_SIZE))
        {
            //printf("FILE LIST\n");
            request->requestCode = ServerRequest_FILE_LIST;
            result = true;
        }
        else if (cant_argumentos == 4 && !strncmp(token, DOWN_CMD, ARGUMENT_SIZE))
        {
            //printf("FILE DOWNLOAD\n");
            request->requestCode = ServerRequest_FILE_DOWNLOAD;
            token = strtok_r(temp_buffer, " ", &temp_buffer);
            snprintf(request->second_argument, ARGUMENT_SIZE, "%s", token);
            token = strtok_r(temp_buffer, " ", &temp_buffer);
            snprintf(image_name, ARGUMENT_SIZE, "%s", token);
            result = true;
        }
    }
    //free(temp_buffer);
    return result;
}

/**
 * @brief Varía la cantidad de argumentos a mostrar de acuerdo al tipo de respuesta.
 * 
 * @param response Tipo de respuesta a imprimir por pantalla
 */
void printResponse(struct Server_Response *response)
{
    switch (response->responseCode)
    {
    case ServerResponse_LOGIN_SUCCESS:
        printf("%s\n", "LOGIN SUCCESS");
        break;
    case ServerResponse_LOGIN_FAIL:
        printf("%s\n", "LOGIN FAILED");
        break;
    case ServerResponse_LOGIN_REJECTED:
        printf("%s\n", "LOGIN REJECTED");
        break;
    case ServerResponse_PASSWD_SUCCESS:
        printf("%s\n", "PASSWORD HAS BEEN CHANGED");
        break;
    case ServerResponse_PASSWD_FAILED:
        printf("%s\n", "PASSWORD COULD NOT BEEN CHANGED");
        break;
    case ServerResponse_LOGOUT:
        printf("%s\n", "LOGOUT");
        break;
    case ServerResponse_CONTINUE:
        printf("%10s  %10s  %20s\n", response->first_argument, response->second_argument, response->third_argument);
        break;
    case ServerResponse_FINISH:
        printf("%10s  %10s  %20s\n", response->first_argument, response->second_argument, response->third_argument);
        break;
    case ServerResponse_FILE_CONTINUE:
        printf("%32s  %10s\n", response->first_argument, response->second_argument);
        print_md5_sum((unsigned char *)response->third_argument);
        break;
    case ServerResponse_FILE_FINISH:
        break;
    default:
        printf("%s\n", "NOT DEFINED");
        break;
    }
    return;
}
