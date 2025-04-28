#include "request.h"

#include <stdio.h>
#include <string.h>

// Parse the HTTP request string into an HttpRequest structure
int parse_http_request(const char *request, HttpRequest *http_request) {
    return sscanf(request, "%7s %255s %15s", http_request->method,
                  http_request->path, http_request->version);
}
