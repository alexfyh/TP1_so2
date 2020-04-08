#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "csv_handler.c"

#define users_db "users.csv"
#define user_column 0
#define password_column 1
#define enabled_column 2
#define date_column 3

//TODO implementar strnlen en vez de strlen

int8_t get_userPosition(char *user)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    char *row_user;
    int8_t i = 0;
    for (int8_t i = 0; i < csv->rows; i++)
    {
        row_user = csv_get(csv, user_column, i);
        if (strlen(user) == strlen(row_user) && !(strncmp(user, row_user, strlen(user))))
        {
            return i;
        }
    }
    return -1;
}

bool isAuthorized(char *user, char *password)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    int8_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    char *password_db = csv_get(csv, password_column, row_number);
    uint8_t enabled = atoi(csv_get(csv, enabled_column, row_number));

    if (strlen(password) == strlen(password_db) && !(strncmp(password, password_db, strlen(password_db))) && enabled)
    {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        //char time_string [20] = {0};
        //strncpy(time_string,asctime(tm),19); 
        //time_string[strcspn(time_string, "\n")] = 0;
        csv_set(csv,date_column,row_number,strtok(asctime(tm), "\n"));
        csv_save(csv,users_db);
        return true;
    }
    return false;
}

int8_t setUserPassword(char *user,char *password){
    //TODO ver límite definido por la estrucutra de los argumentos del servidor
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    int8_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    csv_set(csv,password_column,row_number,password);
    csv_save(csv,users_db);
}

int main(int argc, char *argv[])
{

    printf("%d\n", isAuthorized("user_1", "passwd_2"));
    printf("%d\n", isAuthorized("user_2", "passwd_2"));
    printf("%d\n", isAuthorized("user_3", "passwd_3"));
    setUserPassword("user_10","passwd_01");
    return 0;
}

/*
Limitaciones de la DB:
Una fila puede tener máximo de 1023 caracteres
Si un nombre de usuario se repite, sólo tomará en cuenta el primero encontrado
Los campos no pueden ser vacíos (,,), sí soporta espacios en blanco

Si le pongo la restriccion de cierto tamaño con la funcion
strnlen, puedo evitar overflow del buffer.
*/