#include <stdbool.h>
#include <stdint.h>

#define FIELD_SIZE 32
//typedef struct UserInfo UserInfo;
typedef struct UserInfo
{
    char name[FIELD_SIZE];
    char enabled[FIELD_SIZE];
    char date[FIELD_SIZE];
}UserInfo;

bool isAuthorized(char *,char *);
bool setUserPassword(char *,char *);
uint32_t getUsersCount();
struct UserInfo getUserInfoByRowNumber(uint32_t);