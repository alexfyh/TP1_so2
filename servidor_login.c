#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "login_functions.h"

int main(int argc, char *argv[]){

    bool resultado = isAuthorized(argv[1],argv[2]);

    printf("Resultado %d\n",resultado);
    char * nulo = "01234567890";
    printf("Tamano = %ld\n",strnlen(nulo,11)); 
    return 0;
}
