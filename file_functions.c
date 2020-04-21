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

void printPartitionTable(struct _MBR *MBR)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (MBR->PartTable[i].EndLBA)
		{
			printf("Size of Partition  %u  = %u\n", i, MBR->PartTable[i].EndLBA * SECTOR_SIZE);
			printf("Type of Partition  %u  = %X\n", i, MBR->PartTable[i].PartType);
			printf("Start of Partition %u  = %u\n", i, MBR->PartTable[i].StartLBA * SECTOR_SIZE);
			printf("Booteable of Partition  %u  = %u\n", i, MBR->PartTable[i].status);
			printf("\n");
		}
	}
}
/**
 * TODO = del MBR, tomar la partición booteable, devolver su offset y tamaño y usar un file descriptor
 *  con ese offset y el tamaño de l partición y luego ahcer el checksum!
 * 
 */

int8_t getBooteablePartition(struct _MBR *MBR)
{
	int8_t index = -1;
	for (int8_t i = 0; i < 4; i++)
	{
		if (MBR->PartTable[i].status == 0x80)
		{
			index = i;
		}
	}
	return index;
}

