#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

void send_mod(int32_t, const void *, size_t, int32_t);
void recv_mod(int32_t, void *, size_t, int32_t);
void write_mod(int32_t, const void *buf, size_t);
void read_mod(int32_t, void *buf, size_t);

int32_t setUpConnection(struct sockaddr_in *, char *, int32_t);
int32_t acceptConnection(int32_t, struct sockaddr *);
int32_t connectToServer(char *,char *);
bool str_to_uint16(const char *, uint16_t *);
