#include <stdbool.h>

uint8_t getArgumentsCount(char *);
bool formatRequest(char *, struct Server_Request *);
void printResponse(struct Server_Response*);