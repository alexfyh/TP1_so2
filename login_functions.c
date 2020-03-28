#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define users_db "users.csv"

bool isAuthorized(char *user, char *password)
{
    //https://stackoverflow.com/questions/12911299/read-csv-file-in-c
    FILE *fp = fopen(users_db, "r");
    if (!fp)
    {
        perror(users_db);
        exit(1);
    }
    char line[1024];
    while (fgets(line, 1024, fp))
    {
        if (line[strlen(line) - 1] == '\n')
        {
            line[strlen(line) - 1] = '\0';
        }
        //  TODO = ver ese aviso de que modifica el primer argumento STRTOK
        //  strdup
        char *user_field = strtok(line, ",");
        //  TODO = Ver cuando retorno de strtok es NULL
        if (strlen(user_field) == strlen(user) && !(strncmp(user_field, user, strlen(user_field))))
        {
            char * password_field = strtok(NULL,",");
            if (strlen(password_field) == strlen(password) && !(strncmp(password_field, password, strlen(password_field))))
            {
                return true;
            }
            else
            {
                break;
            }
        }
        /*
        while (campo!=NULL)
        {
            printf("Campo = %s  ---- ",campo);
            campo = strtok(NULL,",");
        }
        */
    }
    fclose(fp);

    return false;
}

void setUnauthorized(char *user)
{
    return;
}
/*
const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ";");
            tok && *tok;
            tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
*/

/*
Limitaciones de la DB:
Una fila puede tener máximo de 1023 caracteres
Si un nombre de usuario se repite, sólo tomará en cuenta el primero encontrado
Los campos no pueden ser vacíos (,,)
*/