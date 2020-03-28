#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "login_functions.h"

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ";\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main(int argc, char *argv[]){

    bool resultado = isAuthorized("Hola","Chau");

    return 0;

    //printf("Resultado %d\n",resultado);
    /*
    FILE* stream = fopen("users.csv", "r");
    char line[1024];
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        printf("Field  would be %s\n", getfield(tmp, 1));
        // NOTE strtok clobbers tmp
        free(tmp);
    }
    */
}
