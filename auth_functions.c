#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "csv_handler.c"
#include "auth_functions.h"

#define users_db "users.csv"
#define user_column     0
#define password_column 1
#define enabled_column  2
#define date_column     3
#define ENANBLED_STRING "ENABLED"
#define DISABLED_STRING "DISABLED"

//TODO implementar strnlen en vez de strlen
//  TODO = tratar de borrar ese casteo explícito hecho.
int32_t get_userPosition(char *user)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    char *row_user;
    for (int32_t i = 0; i < (int32_t)csv->rows; i++)
    {
        row_user = csv_get(csv, user_column, (uint32_t)i);
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
    int32_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    char *password_db = csv_get(csv, password_column,(uint32_t)row_number);
    int32_t enabled = atoi(csv_get(csv, enabled_column, (uint32_t)row_number));

    if (strlen(password) == strlen(password_db) && !(strncmp(password, password_db, strlen(password_db))) && enabled)
    {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        //char time_string [20] = {0};
        //strncpy(time_string,asctime(tm),19); 
        //time_string[strcspn(time_string, "\n")] = 0;
        csv_set(csv,date_column,(uint32_t)row_number,strtok(asctime(tm), "\n"));
        csv_save(csv,users_db);
        return true;
    }
    return false;
}

bool setUserPassword(char *user,char *password){
    //TODO ver límite definido por la estrucutra de los argumentos del servidor
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    int32_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    csv_set(csv,password_column,(uint32_t)row_number,password);
    csv_save(csv,users_db);
    return true;
}

uint32_t getUsersCount(){
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    return csv->rows;
}

struct UserInfo getUserInfoByRowNumber(uint32_t row){
    struct UserInfo userInfo;
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    strncpy(userInfo.name,csv_get(csv,user_column,row),FIELD_SIZE);
    if (atoi(csv_get(csv,enabled_column,row)))
    {
        strncpy(userInfo.enabled,ENANBLED_STRING,FIELD_SIZE);
    }
    else
    {
        strncpy(userInfo.enabled,DISABLED_STRING,FIELD_SIZE);
    }
    strncpy(userInfo.date,csv_get(csv,date_column,row),FIELD_SIZE);
    return userInfo;
}