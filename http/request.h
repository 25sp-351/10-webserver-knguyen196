#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
    char method[8];
    char path[256];
    char version[16];
} HttpRequest;

int parse_http_request(const char *request, HttpRequest *http_request);

#endif  // REQUEST_H
