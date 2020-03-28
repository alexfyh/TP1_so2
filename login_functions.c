#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define users_db "users.csv"
#define USER_COLUMN 0
#define PASSWORD_COLUMN 1
#define ENABLED_COLUMN 2

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
        if(line[strlen(line)-1]=='\n'){
            line[strlen(line)-1]='\0';
        }
        //  TODO = ver ese aviso de que modifica el primer argumento STRTOK
        char * campo = strtok(line,",");
        while (campo!=NULL)
        {
            printf("Campo = %s  ---- ",campo);
            campo = strtok(NULL,",");
        } 
        printf("\n");
    }
    fclose(fp);
    return true;
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
*/