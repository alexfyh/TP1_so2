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

    bool resultado = isAuthorized(argv[1],argv[2]);

    printf("Resultado %d\n",resultado);
    return 0;
}
