

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define users_db "users.csv"
#define FIELD_LENGTH 25
#define LINE_LENGTH 100

bool isAuthorized(char *user, char *password)
{
    if (user == NULL || password == NULL)
    {
        perror("Usuario y/o password no puede ser NULL");
        return false;
    }
    FILE *fp = fopen(users_db, "r");
    if (!fp)
    {
        perror(users_db);
        exit(1);
    }
    char line[LINE_LENGTH];
    while (fgets(line, LINE_LENGTH, fp))
    {
        if (line[strnlen(line, LINE_LENGTH) - 1] == '\n')
        {
            line[strnlen(line, LINE_LENGTH) - 1] = '\0';
        }
        //  TODO = ver ese aviso de que modifica el primer argumento STRTOK(strdup)
        char *user_field = strtok(line, ",");
        if (user_field == NULL)
        {
            continue;
        }
        if (strnlen(user_field, FIELD_LENGTH) == strnlen(user, FIELD_LENGTH) && !(strncmp(user_field, user, strnlen(user_field, FIELD_LENGTH))))
        {
            char *password_field = strtok(NULL, ",");
            if (user_field == NULL)
            {
                return false;
            }
            if (strnlen(password_field, FIELD_LENGTH) == strnlen(password, FIELD_LENGTH) && !(strncmp(password_field, password, strnlen(password_field, FIELD_LENGTH))))
            {
                return true;
            }
            else
            {
                break;
            }
        }
    }
    if (fclose(fp) == EOF)
    {
        //fflush(fp); //Es necesario si sólo estoy leyendo?
        perror("Error al cerrar archivo");
    }
    return false;
}
/*
#include <time.h>


time_t t = time(NULL);
struct tm *tm = localtime(&t);
printf("%s", asctime(tm));
FILE *fp = fopen("log","a");
                    fprintf(fp, "EXITO=%s,%s,%s",request->first_argument,request->second_argument,asctime(tm));
                    response->code = Auth_SUCCESS;
                    fclose(fp);

*/

/*
void setUnAuthorized(char *user)
{
    return;
}
*/

/*
Limitaciones de la DB:
Una fila puede tener máximo de 1023 caracteres
Si un nombre de usuario se repite, sólo tomará en cuenta el primero encontrado
Los campos no pueden ser vacíos (,,), sí soporta espacios en blanco

Si le pongo la restriccion de cierto tamaño con la funcion
strnlen, puedo evitar overflow del buffer.


Si algún campo tiene más de 19 caracteres,se da en la pera las comprobaciones de login
*/

//  Fuente de inspiración : https://stackoverflow.com/questions/12911299/read-csv-file-in-c