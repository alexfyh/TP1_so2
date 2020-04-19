#include <stdbool.h>
#include <stdint.h>

#define FIELD_SIZE 32
#define ARGUMENT_SIZE 32

typedef struct UserInfo
{
    char name[FIELD_SIZE];
    char enabled[FIELD_SIZE];
    char date[FIELD_SIZE];
} UserInfo;

typedef enum
{
    Auth_LOGIN,
    Auth_LIST,
    Auth_PASSWD
} Auth_Request_Code;

typedef enum
{
    Auth_SUCCESS,
    Auth_FAIL,
    Auth_CONTINUE,
    Auth_FINISH,
    Auth_LIST_FAIL
} Auth_Response_Code;

typedef struct Auth_Request
{
    Auth_Request_Code code;
    char first_argument[ARGUMENT_SIZE];
    char second_argument[ARGUMENT_SIZE];
} Auth_Request;

typedef struct Auth_Response
{
    Auth_Response_Code code;
    char first_argument[ARGUMENT_SIZE];
    char second_argument[ARGUMENT_SIZE];
    char third_argument[ARGUMENT_SIZE];
} Auth_Response;

bool isAuthorized(char *, char *);
bool setUserPassword(char *, char *);
uint32_t getUsersCount();
struct UserInfo *getUserInfoByRowNumber(uint32_t);
int32_t get_userPosition(char *);

