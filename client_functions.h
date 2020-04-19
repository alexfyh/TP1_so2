#include <stdbool.h>
#include <stdint.h>

uint8_t getArgumentsCount(char *);
bool formatRequest(char *, uint32_t,struct Server_Request *,char *);
void printResponse(struct Server_Response*);
