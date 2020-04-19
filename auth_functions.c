#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "csv_handler.c"
#include "auth_functions.h"

#define user_column 0
#define password_column 1
#define enabled_column 2
#define date_column 3
#define ENANBLED_STRING "ENABLED"
#define DISABLED_STRING "DISABLED"
static const char *users_db = "users.csv";

/**
 * @brief Devuelve la posición (fila) del usuario en el archivo csv
 * 
 * @param user Usuario a buscar
 * @return int32_t valor de la posición, -1 si no existe el usuario
 */
int32_t get_userPosition(char *user)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    //  TODO = Analizar qué pasa si la cantida de líneas va más alla del límite signed
    for (int32_t i = 0; i < (int32_t)csv->rows; i++)
    {
        char *row_user;
        row_user = csv_get(csv, user_column, (uint32_t)i);
        if (strlen(user) == strlen(row_user) && !(strncmp(user, row_user, strlen(user))))
        {
            return i;
        }
    }
    return -1;
}
/**
 * @brief Devuelve el resultado del intento login de un usuario.
 * Si la solicitud de logueo ha sido exitosa, se guarda la fecha de login en el csv.
 * 
 * @param user El nombre de usuario a buscar
 * @param password La contraseña asociada al usuario
 * @return true Credenciales y estado del usuario válidos
 * @return false Credenciales y/o estado del no son válidos
 */
bool isAuthorized(char *user, char *password)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    int32_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    char *password_db = csv_get(csv, password_column, (uint32_t)row_number);
    int32_t enabled = atoi(csv_get(csv, enabled_column, (uint32_t)row_number));
    if (strnlen(password, ARGUMENT_SIZE) == strnlen(password_db, ARGUMENT_SIZE) && !(strncmp(password, password_db, strnlen(password_db, ARGUMENT_SIZE))) && enabled)
    {
        time_t rawtime;
        struct tm *info;
        time(&rawtime);
        info = localtime(&rawtime);
        char date[ARGUMENT_SIZE] = {0};
        strftime(date, ARGUMENT_SIZE, "%c", info);
        csv_set(csv, date_column, (uint32_t)row_number, date);
        csv_save(csv, users_db);
        return true;
    }
    return false;
}

/**
 * @brief Setea el password del usuario en el csv
 * 
 * @param user Usuario al cual se le va a cambiar la password 
 * @param password Nueva contraseña
 * @return true Se ha cambiado la contrasña exitosa
 * @return false No se ha encontrado el usuario
 */
bool setUserPassword(char *user, char *password)
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    int32_t row_number = get_userPosition(user);
    if (row_number == -1)
    {
        return false;
    }
    csv_set(csv, password_column, (uint32_t)row_number, password);
    csv_save(csv, users_db);
    return true;
}

/**
 * @brief Devuelve la cantidad de usuario en el csv
 * Devuelve -1 en caso de error.
 * 
 * @return uint32_t Cantidad de usuarios existentes en el csv 
 */
uint32_t getUsersCount()
{
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    return csv->rows;
}

/**
 * @brief Devuelve una estructura UserInfo según la fila seleccionada
 * 
 * @param row Número de fila correspondiente al usuario
 * @return struct UserInfo que contien nombre,estado y fecha de último login
 * En caso que no se pueda retornar la info del usuario, devuelve NULL  
 */
struct UserInfo *getUserInfoByRowNumber(uint32_t row)
{
    struct UserInfo *userInfo = (struct UserInfo *) calloc(1,sizeof(struct UserInfo));
    CSV *csv = csv_create(0, 0);
    csv_open(csv, users_db);
    if (row>csv->rows)
    {
        free(userInfo);
        return NULL;
    }
    snprintf(userInfo->name, ARGUMENT_SIZE, "%s", csv_get(csv, user_column, row));
    if (atoi(csv_get(csv, enabled_column, row)))
    {
        snprintf(userInfo->enabled, ARGUMENT_SIZE, "%s", ENANBLED_STRING);
    }
    else
    {
        snprintf(userInfo->enabled, ARGUMENT_SIZE, "%s", DISABLED_STRING);
    }
    snprintf(userInfo->date, ARGUMENT_SIZE, "%s", csv_get(csv, date_column, row));
    return userInfo;
}
