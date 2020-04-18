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
#include <sys/mman.h>
#include <sys/sendfile.h>

#include "file_functions.h"
#include "transactions.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define ISO_PATH "isos"
#define ARGUMENT_SIZE 32

//  COMPILAR CON:   gcc -o file_functions file_functions.c -D_DEFAULT_SOURCE
//              :   gcc -o file_functions file_functions.c -D_BSD_SOURCE
//  Instalacion de opensll : apt-get install libssl-dev
// compilar con -lcrypto -lssl
//  Con directivas de compilador (IFDEF) borrar el main
// https://stackoverflow.com/questions/3395690/md5sum-of-file-in-linux-c
//  https://ftp1.digi.com/support/documentation/0220055_d.pdf
//  https://rosettacode.org/wiki/MD5

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("File descriptors needed!!!\n");
        //perror("File descriptors needed!!!");
        exit(EXIT_FAILURE);
    }
    int32_t fd_read = atoi(argv[1]);
    int32_t fd_write = atoi(argv[2]);
    if (!fd_read || !fd_write)
    {
        perror("Casteo de descriptores fallido");
        //  TODO = debería avisar al ppid que fallo y finalizarlo
        exit(EXIT_FAILURE);
    }
    struct File_Request *request = (struct File_Request *) calloc(1, sizeof(struct File_Request));
    struct File_Response *response = (struct File_Response *) calloc(1, sizeof(struct File_Response));
    ssize_t n;
    DIR *dp;
    struct dirent *ep;
    unsigned char result[MD5_DIGEST_LENGTH];
    if (chdir(ISO_PATH) != 0)
    {
        perror("No se pudo acceder al directorio\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        n = read(fd_read, request, sizeof(struct File_Request));
        if (n != sizeof(struct File_Request))
        {
            perror("No se ha leído la estructura correctamente");
            response->code = File_FAIL;
        }
        else
        {
            switch (request->code)
            {
            case File_LIST:
                dp = opendir(".");
                if (!dp)
                {
                    perror("No se pudo acceder al directorio_larar");
                    response->code = File_FAIL;
                }
                while ((ep = readdir(dp)) != NULL)
                {
                    if (ep->d_type == DT_REG)
                    {
                        char *file_buffer;
                        int32_t file_descript = open(ep->d_name, O_RDONLY);
                        if (file_descript < 0)
                        { //Si fallo un archivo, que continue
                            perror("File descriptor");
                            continue;
                        }
                        int64_t file_size = get_size_by_fd(file_descript);
                        if (file_size < 0)
                        { //Si fallo un archivo, que continue
                            perror("Tamaño no valido");
                            continue;
                        }
                        file_buffer = mmap(0, (uint64_t)file_size, PROT_READ, MAP_SHARED, file_descript, 0);
                        //TODO casteo implícito porque sino me hubiera salido antes
                        MD5((unsigned char *)file_buffer, (uint64_t)file_size, result);
                        munmap(file_buffer, (uint64_t)file_size);
                        response->code = File_LIST_CONTINUE;
                        strncpy(response->first_argument, ep->d_name, ARGUMENT_SIZE);
                        char str_file_size[ARGUMENT_SIZE];
                        strncpy(response->second_argument, readable_fs(file_size, str_file_size), ARGUMENT_SIZE);
                        memcpy(response->third_argument, result, MD5_DIGEST_LENGTH);
                        write_mod(fd_write, response, sizeof(struct File_Response));
                    }
                }
                response->code = File_LIST_FINISH;
                write_mod(fd_write, response, sizeof(struct File_Response));
                closedir(dp);
                break;
            case File_DOWNLOAD: ;
                //printf("%s\n",request->second_argument);
                //uint16_t port = (uint16_t)atoi(request->second_argument);
                int32_t file_socket = connectToServer(request->first_argument,request->second_argument);
                //TODO =  Manejo de errores del descitpr
                int32_t booteable_fd = open(request->third_argument,O_RDONLY);
                struct stat stat_buf;
                fstat(booteable_fd,&stat_buf);
                sendfile (file_socket, booteable_fd, NULL,(uint64_t) stat_buf.st_size);
                close(booteable_fd);
                close(file_socket);
                break;
            }
        }
        n = write(fd_write, response, sizeof(struct File_Response));
        if (n != sizeof(struct File_Response))
        {
            //  TODO = cómo se debería comportar teniendo en cuenta que
            //  del otro lado se quedó colgado escuchando ???
            perror("No se ha escrito  correctamente la respuesta");
        }
    }
    free(request);
    free(response);
    exit(EXIT_FAILURE);
}
