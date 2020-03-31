extern const size_t BUFFER_SIZE;
extern const char *ACCESS_DENIED;
extern const char *LOGIN_INCORRECT;
extern const char *LOGIN;
extern const char *PASSWORD;

void write_buffer(int32_t, const char *, char *);
char *read_buffer(int32_t, char *);