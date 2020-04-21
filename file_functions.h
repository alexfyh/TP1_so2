#include <stdint.h>
#define ARGUMENT_SIZE 32

typedef enum
{
    File_LIST,
    File_DOWNLOAD
} File_Request_Code;

typedef enum
{
    File_LIST_CONTINUE,
    File_LIST_FINISH,
    File_DOWNLOAD_COMPLETE,
    File_DOWNLOAD_ABORTED,
    File_FAIL
} File_Response_Code;

typedef struct File_Request
{
    File_Request_Code code;
    char first_argument[ARGUMENT_SIZE];
    char second_argument[ARGUMENT_SIZE];
    char third_argument[ARGUMENT_SIZE];
} File_Request;

typedef struct File_Response
{
    File_Response_Code code;
    char first_argument[ARGUMENT_SIZE];
    char second_argument[ARGUMENT_SIZE];
    char third_argument[ARGUMENT_SIZE];
} File_Response;


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

int64_t get_size_by_fd(int);
void print_md5_sum(unsigned char *);
char* readable_fs(int64_t, char *,uint32_t);
void printPartitionTable(struct _MBR *);
int8_t getBooteablePartition(struct _MBR *);


