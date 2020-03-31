#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

const size_t BUFFER_SIZE = 250;
const char *ACCESS_DENIED = "ACCESS DENIED";
const char *LOGIN_INCORRECT = "LOGIN INCORRECT";
const char *LOGIN = "login:";
const char *PASSWORD="password:";

//  TODO redefinir el TAM como BUFFER_SIZE acá en vez del servidor y cliente
//  y así no es encesario pasarle el parámetro int  buffer size acá

void write_buffer(int32_t socket, const char *source, char *buffer)
{
    ssize_t n;
    memset(buffer, '\0', BUFFER_SIZE);
    if (source)
    {
        n = write(socket, source, BUFFER_SIZE - 1);
    }
    else
    {
        //TODO = ver como solucionar la negrada de casteo que hago
        fgets(buffer, (int32_t)BUFFER_SIZE - 1, stdin);
        n = write(socket, buffer, BUFFER_SIZE - 1);
    }

    if (n < 0)
    {
        perror("escritura de socket");
        exit(1);
    }
}


//  TODO = Ver si es necesario devolver el puntero, ya que el buffer se puede acceder por afuera
char *read_buffer(int32_t socket, char *buffer)
{
    //memset(buffer, '\0', BUFFER_SIZE);
    ssize_t n;
    n = read(socket, buffer, BUFFER_SIZE - 1);
    if (n < 0)
    {
        perror("lectura de socket");
        exit(1);
    }
    return buffer;
}