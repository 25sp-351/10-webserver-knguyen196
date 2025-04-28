#include "response.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Sends a simple HTTP response with the given body
void send_http_response(int client_fd, const char *body) {
    char response[1024];
    int content_length = strlen(body);
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n",
             content_length);
    write(client_fd, response, strlen(response));
    write(client_fd, body, content_length);
}

// Sends a 404 Not Found response
void send_404_response(int client_fd) {
    const char *body = "404 Not Found";
    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 404 Not Found\r\n"
             "Content-Type: text/plain\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n\r\n",
             strlen(body));
    write(client_fd, response, strlen(response));
    write(client_fd, body, strlen(body));
}
