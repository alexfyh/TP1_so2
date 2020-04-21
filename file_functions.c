#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "file_functions.h"

/**
 * @brief Get the size by fd object
 * 
 * @param fd 
 * @return int64_t 
 */
int64_t get_size_by_fd(int fd)
{
    struct stat statbuf;
    if (fstat(fd, &statbuf) < 0)
        return -1;
    return statbuf.st_size;
}

void print_md5_sum(unsigned char *md)
{
    int i;
    for (i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        printf("%02x", md[i]);
    }
}

char* readable_fs(int64_t size, char *buf,uint32_t buffer_size) {
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    snprintf(buf, buffer_size,"%.*ld %s", i, size, units[i]);
    return buf;
}
