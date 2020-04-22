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
	uint8_t status;
	uint8_t StartAddrHead;
	uint16_t StartAddrCylSec;
	uint8_t PartType;
	uint8_t EndAddrHead;
	uint16_t EndAddrCylSec;
	uint32_t StartLBA;
	uint32_t EndLBA;
} __attribute__((__packed__)) Partition;

typedef struct _MBR
{
	uint8_t Code[440];
	uint32_t DiskSig;
	uint16_t Reserved;
	struct _Partition PartTable[4];
	uint8_t BootSignature[2];
} __attribute__((__packed__)) MBR;

int64_t get_size_by_fd(int);
void print_md5_sum(unsigned char *);
char* readable_fs(int64_t, char *,uint32_t);
void printPartitionTable(struct _MBR *);
int8_t getBooteablePartition(struct _MBR *);
int8_t getMD5Hash(uint64_t , int32_t , uint32_t , unsigned char *);



