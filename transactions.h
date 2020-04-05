#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

void send_mod(int32_t, const void *, size_t, int32_t);
void recv_mod(int32_t, const void *, size_t, int32_t);
void write_mod(int32_t, void *buf, size_t);
void read_mod(int32_t, void *buf, size_t);