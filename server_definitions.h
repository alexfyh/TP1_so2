#define ARGUMENT_SIZE 32

typedef enum Server_Request_Code{
    Server_LOGIN,
    Server_USER_LIST,
    Server_PASSWD,
    Server_FILE_LIST,
    SERVER_DOWN
} Server_Request_Code;

typedef enum Server_Response_Code{
    Server_LOGIN_SUCCESS,
    Server_LOGIN_FAIL,
    Server_LOGIN_REJECTED,
    Server_CONTINUE
} Server_Response_Code;

typedef struct Server_Request
{
    Server_Request_Code code;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
}Server_Request;

typedef struct Server_Response
{
    Server_Response_Code code;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
    char third_argument [ARGUMENT_SIZE];
}Server_Response;
