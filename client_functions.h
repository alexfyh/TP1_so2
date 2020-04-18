#include <stdbool.h>
#include <stdint.h>

uint8_t getArgumentsCount(char *);
bool formatRequest(char *, struct Server_Request *,char *);
void printResponse(struct Server_Response*);
