#include <stdio.h>

int main(){
    printf("%ld\n",sizeof(__DATE__));
    printf("%ld\n",sizeof(__TIME__));
    char cadena [32] = {0};
    snprintf(cadena,32,"%s  %s",__DATE__,__TIME__);
    printf("%s\n",cadena);
    
}