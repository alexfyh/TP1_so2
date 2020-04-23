#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include "file_functions.h"
#include <sys/mman.h>
/**
 * @brief Devuelve el tamaño del archivo del file descriptor
 * 
 * @param fd Descriptor de archivo
 * @return int64_t Tamaño del archivo
 */
int64_t get_size_by_fd(int fd)
{
	struct stat statbuf;
	if (fstat(fd, &statbuf) < 0)
		return -1;
	return statbuf.st_size;
}

/**
 * @brief Imprime en pantalla el Hash MD5 en hexa
 * 
 * @param md Arreglo de char donde está almacenado el hash
 */
void print_md5_sum(unsigned char *md)
{
	printf("MD5 HASH = ");
	int i;
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		printf("%02x", md[i]);
	}
	printf("\n");
}

/**
 * @brief Devuelve en formato human readable el tamaño en bytes pasado.
 * 
 * @param size Tamaño en bytes
 * @param buf Buffer donde escribir el resultado
 * @param buffer_size Tamaño del buffer pasado
 * @return char* String en formato human readable
 */
char *readable_fs(int64_t size, char *buf, uint32_t buffer_size)
{
	int i = 0;
	const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (size > 1024)
	{
		size /= 1024;
		i++;
	}
	snprintf(buf, buffer_size, "%.*ld %s", i, size, units[i]);
	return buf;
}

/**
 * @brief Imprimer en pantalla el tamaño , el Tipo, inicio de cada partición primaria
 * de la estructura Master Boot Record pasada.
 * 
 * @param MBR estructura MBR.
 */
void printPartitionTable(struct _MBR *MBR)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		if (MBR->PartTable[i].EndLBA)
		{
			char printable[ARGUMENT_SIZE] = {0};
			printf("Size of Partition  %u  = %s\n", i, readable_fs((uint64_t)(MBR->PartTable[i].EndLBA * SECTOR_SIZE), printable, ARGUMENT_SIZE));
			printf("Type of Partition  %u  = %X\n", i, MBR->PartTable[i].PartType);
			printf("Start of Partition %u  = %s\n", i, readable_fs((uint64_t)(MBR->PartTable[i].StartLBA * SECTOR_SIZE), printable, ARGUMENT_SIZE));
			printf("Booteable of Partition  %u  = %u\n", i, MBR->PartTable[i].status);
			printf("\n");
		}
	}
}

/**
 * @brief Obtiene el índice de la partición booteable ubicado dentro
 * de la tabla de particiones del MBR
 * 
 * @param MBR Master Boot Record 
 * @return int8_t índice de la partición booteable. -1 en caso de error.
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

/**
 * @brief Guarda en el result el resultado del hash, a partir del descriptor de archivo device_fd
 * 
 * @param size Tamaño del archivo o bloque aplicar la función hash
 * @param device_fd Descriptor de archivo al archivo sobre el cual hacer hash.
 * @param offset Desde qué byte leer el archivo
 * @param result Buffer donde se guarda el Hash
 * @return int8_t Resultado del hash, -1 si se produjo error.
 */
int8_t getMD5Hash(uint64_t size, int32_t device_fd, uint32_t offset, unsigned char *result)
{
	char *file_buffer;
	file_buffer = mmap(NULL, size, PROT_READ, MAP_SHARED, device_fd, offset);
	if (file_buffer==MAP_FAILED)
	{
		return -1;
	}
	MD5((unsigned char *)file_buffer, size, result);
	munmap(file_buffer, (uint64_t)size);
	return 1;
}

