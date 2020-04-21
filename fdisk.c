#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <inttypes.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <sys/mman.h>


#define SECTOR_SIZE 512

typedef struct _Partition
{
	uint8_t status;			  //0x80 for bootable, 0x00 for not bootable, anything else for invalid
	uint8_t StartAddrHead;	  //head address of start of partition
	uint16_t StartAddrCylSec; //(AddrCylSec & 0x3F) for sector,  (AddrCylSec & 0x3FF) for cylendar
	uint8_t PartType;
	uint8_t EndAddrHead;	//head address of start of partition
	uint16_t EndAddrCylSec; //(AddrCylSec & 0x3F) for sector,  (AddrCylSec & 0x3FF) for cylendar
	uint32_t StartLBA;		//linear address of first sector in partition. Multiply by sector size (usually 512) for real offset
	uint32_t EndLBA;		//linear address of last sector in partition. Multiply by sector size (usually 512) for real offset
} __attribute__((__packed__)) Partition;

typedef struct _MBR
{
	uint8_t Code[440];
	uint32_t DiskSig;  //This is optional
	uint16_t Reserved; //Usually 0x0000
	struct _Partition PartTable[4];
	uint8_t BootSignature[2]; //0x55 0xAA for bootable
} __attribute__((__packed__)) MBR;

void printPartitionTable(struct _MBR *);
int8_t getBooteablePartition(struct _MBR *);
int main()
{
	int32_t fd_device = open("/dev/sdb", O_RDONLY);
	printf("%d\n", fd_device);
	struct _MBR MBR;
	read(fd_device, &MBR, sizeof(struct _MBR));
	printPartitionTable(&MBR);
	uint8_t bootable_index = getBooteablePartition(&MBR);

	int offset = MBR.PartTable[bootable_index].StartLBA * SECTOR_SIZE;

	//fd_device = open("/dev/sdb", O_RDONLY);
	unsigned char result[MD5_DIGEST_LENGTH];
	char *file_buffer;
	if (fd_device < 0)
	{ //Si fallo un archivo, que continue
		perror("File descriptor");
		exit(1);
	}
	int64_t file_size = get_size_by_fd(fd_device);
	printf("file size %ld\n",file_size);
	if (file_size < 0)
	{
		perror("Tamaño no valido");
	}
	file_buffer = mmap(0, 680525824, PROT_READ, MAP_SHARED, fd_device, 0);
	//TODO casteo implícito porque sino me hubiera salido antes
	MD5((unsigned char *)file_buffer, 680525824, result);
	munmap(file_buffer, (uint64_t)file_size);
	print_md5_sum(result);

	
	return 0;
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

//gcc fdisk.c file_functions.c  -o fdisk -lcrypto -lssl

//sudo dd if=/dev/sda ibs=512 count=1 | hexdump -C

// calculos de offset
// https://stackoverflow.com/questions/18749349/finding-offset-of-a-structure-element-in-c

/**
 * md5  del isos:
 * e8eeda7b3d609fddf6497f05a60d63d2  archlinux-2020.04.01-x86_64.iso 
 * 
 * Web:
 * e8eeda7b3d609fddf6497f05a60d63d2
 * 
 */