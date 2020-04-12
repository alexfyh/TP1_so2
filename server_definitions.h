#define ARGUMENT_SIZE 32

#define ServerRequest_LOGIN         1
#define ServerRequest_LOGOUT        2
#define ServerRequest_USER_LIST     3
#define ServerRequest_PASSWD        4
#define ServerRequest_FILE_LIST     5
#define ServerRequest_FILE_DOWNLOAD 6

#define ServerResponse_LOGIN_SUCCESS    7
#define ServerResponse_LOGIN_FAIL       8
#define ServerResponse_LOGIN_REJECTED   9
#define ServerResponse_PASSWD_SUCCESS   10
#define ServerResponse_PASSWD_FAILED    11
#define ServerResponse_LOGOUT           12
#define ServerResponse_CONTINUE         13
#define ServerResponse_FINISH           14

typedef struct Server_Request
{
    uint8_t requestCode;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
}Server_Request;

typedef struct Server_Response
{
    uint8_t responseCode;
    char first_argument [ARGUMENT_SIZE];
    char second_argument [ARGUMENT_SIZE];
    char third_argument [ARGUMENT_SIZE];
}Server_Response;
