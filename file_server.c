//  https://www.gnu.org/software/libc/manual/html_node/Simple-Directory-Lister.html

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define ISO_PATH "./isos/"
#define tamano_buffer 250

//  COMPILAR CON:   gcc -o file_functions file_functions.c -D_DEFAULT_SOURCE
//              :   gcc -o file_functions file_functions.c -D_BSD_SOURCE
//  Instalacion de opensll : apt-get install libssl-dev
// compilar con -lcrypto -lssl
//  Con directivas de compilador (IFDEF) borrar el main
// https://stackoverflow.com/questions/3395690/md5sum-of-file-in-linux-c

//  https://rosettacode.org/wiki/MD5

unsigned long get_size_by_fd(int);

int main(int argc, char *argv[])
{
    DIR *dp;
    struct dirent *ep;
    dp = opendir(ISO_PATH);
    char cadena[tamano_buffer];
    memset(cadena, '\0', sizeof(cadena));

    //TODO = Aca se debería quedar escuchando por conexiones el socket???
    if (dp)
    {
        while (ep = readdir(dp))
        {
            if (ep->d_type != DT_DIR)
            {
                /*
                int32_t file_descript;
                //Cambiar working directory
                char * file_name = strcat(ISO_PATH,ep->d_name);
                file_descript = open(file_name, O_RDONLY);
                if (file_descript < 0)
                {
                    exit(EXIT_FAILURE);
                }
                
                unsigned long int file_size = get_size_by_fd(file_descript);
                

                printf("File name= %s\nSize=%ld\n", ep->d_name,file_size);
                */
               char file_name [20]; 
               strcpy(file_name,ISO_PATH);
               strcat(file_name,ep->d_name);
               int32_t file_descript = open(file_name, O_RDONLY);
               unsigned long int file_size = get_size_by_fd(file_descript);
               printf("File name  =%s\n",file_name);
               printf("File sieze =%ld\n",file_size);
               //printf("File name= %s\n", ep->d_name);
            }
        }
        //TODO = acá debería terminar la conexión el socket
        (void)closedir(dp);
    }
    else
    {
        //TODO = acá debería terminar la conexión el socket
        perror("Couldn't open the directory");
    }
    return 0;
}

unsigned long get_size_by_fd(int fd)
{
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0)
        exit(-1);
    return statbuf.st_size;
}