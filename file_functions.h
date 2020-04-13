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
} File_Request;

typedef struct File_Response
{
    File_Response_Code code;
    char first_argument[ARGUMENT_SIZE];
    char second_argument[ARGUMENT_SIZE];
    char third_argument[ARGUMENT_SIZE];
} File_Response;

int64_t get_size_by_fd(int);
void print_md5_sum(unsigned char *);
char* readable_fs(int64_t, char *);
