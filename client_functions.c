#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <malloc.h>

#include "server_definitions.h"

#define LINE_SIZE 130
const char *EXIT_CMD = "exit";
const char *LS_CMD = "ls";
const char *USER_CMD = "user";
const char *PASSWD_CMD = "passwd";
const char *FILE_CMD = "file";
#define PRIu32 "u"

uint8_t getArgumentsCount(char *buffer)
{
    char *temp_buffer = calloc(LINE_SIZE,sizeof(char));
    strncpy(temp_buffer, buffer, LINE_SIZE);
    uint8_t count = 0;
    char *token = strtok(temp_buffer, " ");
    while (token != NULL)
    {
        count++;
        token = strtok(NULL, " ");
    }
    free(temp_buffer);
    return count;
}

bool formatRequest(char *buffer, struct Server_Request *request)
{
    bool result;
    char *temp_buffer = calloc(LINE_SIZE,sizeof(char));
    strncpy(temp_buffer, buffer, LINE_SIZE);
    char *token = strtok_r(temp_buffer, " ", &temp_buffer);
    printf("Primer argumento= %s\n", token);
    uint8_t cant_argumentos = getArgumentsCount(buffer);
    if (cant_argumentos == 1 && strlen(token) == strlen(EXIT_CMD) && !strncmp(token, EXIT_CMD, strlen(EXIT_CMD)))
    {
        printf("Exit command\n");
        request->requestCode = ServerRequest_LOGOUT;
        result = true;
    }
    else if (strlen(token) == strlen(USER_CMD) && !strncmp(token, USER_CMD, strlen(USER_CMD)))
    {
        token = strtok_r(temp_buffer, " ", &temp_buffer);
        printf("Segundo argumento= %s\n", token);
        if (cant_argumentos == 2 && strlen(token) == strlen(LS_CMD) && !strncmp(token, LS_CMD, strlen(LS_CMD)))
        {
            printf("USER LIST\n");
            request->requestCode = ServerRequest_USER_LIST;
            result = true;
        }
        else if (cant_argumentos == 3 && strlen(token) == strlen(PASSWD_CMD) && !strncmp(token, PASSWD_CMD, strlen(PASSWD_CMD)))
        {
            token = strtok_r(temp_buffer, " ", &temp_buffer);
            printf("Tercer argumento= %s\n", token);
            request->requestCode = ServerRequest_PASSWD;
            strncpy(request->first_argument, token, strnlen(token, ARGUMENT_SIZE));
            result = true;
        }
        else
        {
            result = false;
        }
    }
    else if (!strncmp(token, FILE_CMD, strlen(FILE_CMD)))
    {
        printf("File command\n");
        result = true;
    }
    else
    {
        result = false;
    }
    //free(temp_buffer);
    return result;
}

void printResponse(struct Server_Response* response){
    switch (response->responseCode)
    {
    case ServerResponse_LOGIN_SUCCESS:
        printf("%s\n","LOGIN SUCCESS");
        break;
    case ServerResponse_LOGIN_FAIL:
        printf("%s\n","LOGIN FAILED");
        break;
    case ServerResponse_LOGIN_REJECTED:
        printf("%s\n","LOGIN REJECTED");
        break;
    case ServerResponse_PASSWD_SUCCESS:
        printf("%s\n","PASSWORD HAS BEEN CHANGED");
        break;
    case ServerResponse_PASSWD_FAILED:
        printf("%s\n","PASSWORD COULD NOT BEEN CHANGED");
        break;
    case ServerResponse_LOGOUT:
        printf("%s\n","LOGOUT");
        break; 
    case ServerResponse_CONTINUE:
        printf("%s  %s  %s\n",response->first_argument,response->second_argument,response->third_argument);
        break;
    case ServerResponse_FINISH:
        printf("%s  %s  %s\n",response->first_argument,response->second_argument,response->third_argument);
        break;
    default:
        printf("%s\n","NOT DEFINED");
        break;
    }
    return;
}