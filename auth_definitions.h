#define ARGUMENT_SIZE 32

typedef enum{
    Auth_LOGIN,
    Auth_LIST,
    Auth_PASSWD
} Auth_Request_Code;

typedef enum{
    Auth_SUCCESS,
    Auth_FAIL,
    Auth_CONTINUE
} Auth_Response_Code;

typedef struct Auth_Request
{
    Auth_Request_Code code;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
}Auth_Request;

typedef struct Auth_Response
{
    Auth_Response_Code code;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
    char third_argument [ARGUMENT_SIZE];
}Auth_Response;

