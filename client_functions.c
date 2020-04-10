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



uint32_t getArgumentsNumber(char *buffer)
{
    char *temp_buffer = malloc(sizeof(strlen(buffer)));
    strncpy(temp_buffer, buffer, 130);
    uint32_t count = 0;
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
    char *temp_buffer = malloc(LINE_SIZE);
    strncpy(temp_buffer, buffer, LINE_SIZE);
    char *token = strtok_r(temp_buffer, " ", &temp_buffer);
    printf("Primer argumento= %s\n", token);
    uint32_t cant_argumentos = getArgumentsNumber(buffer);
    if (cant_argumentos == 1 && strlen(token) == strlen(EXIT_CMD) && !strncmp(token, EXIT_CMD, strlen(EXIT_CMD)))
    {
        printf("Exit command\n");
        request->code = SERVER_LOGOUT;
        result = true;
    }
    else if (strlen(token) == strlen(USER_CMD) && !strncmp(token, USER_CMD, strlen(USER_CMD)))
    {
        token = strtok_r(temp_buffer, " ", &temp_buffer);
        printf("Segundo argumento= %s\n", token);
        if (cant_argumentos == 2 && strlen(token) == strlen(LS_CMD) && !strncmp(token, LS_CMD, strlen(LS_CMD)))
        {
            printf("USER LIST\n");
            request->code = Server_USER_LIST;
            result = true;
        }
        else if (cant_argumentos == 3 && strlen(token) == strlen(PASSWD_CMD) && !strncmp(token, PASSWD_CMD, strlen(PASSWD_CMD)))
        {
            token = strtok_r(temp_buffer, " ", &temp_buffer);
            printf("Tercer argumento= %s\n", token);
            request->code = Server_PASSWD;
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
    free(temp_buffer);
    return result;
}
