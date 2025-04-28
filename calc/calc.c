#include "calc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../http/response.h"

// Handles the /calc endpoint
void handle_calc(int client_fd, const char *path) {
    char op[8];
    int a, b;
    if (sscanf(path, "/calc/%7[^/]/%d/%d", op, &a, &b) != 3) {
        send_http_response(client_fd, "Invalid calc format");
        return;
    }

    int result;
    if (strncmp(op, "add", 3) == 0 && op[3] == '\0')
        result = a + b;
    else if (strncmp(op, "mul", 3) == 0 && op[3] == '\0')
        result = a * b;
    else if (strncmp(op, "div", 3) == 0 && op[3] == '\0' && b != 0)
        result = a / b;
    else {
        send_http_response(client_fd, "Invalid operation or zero division");
        return;
    }

    char body[64];
    snprintf(body, sizeof(body), "Result: %d", result);
    send_http_response(client_fd, body);
}
