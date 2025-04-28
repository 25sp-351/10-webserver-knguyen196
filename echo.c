#include "echo.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "calc/calc.h"
#include "http/request.h"
#include "http/response.h"
#include "static/static.h"

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 80

// Prints an error message and exits the program
void print_error(const char *msg) {
    perror(msg);
    exit(1);
}

// Parses command line arguments for -p and -v (port and verbose)
// Sets default values if not provided
void parse_args(int argc, char *argv[], int *port, int *verbose) {
    *port    = DEFAULT_PORT;
    *verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) == 2 && strncmp(argv[i], "-p", 2) == 0) {
            if (i + 1 >= argc || sscanf(argv[++i], "%d", port) != 1 ||
                *port <= 0 || *port > 65535) {
                fprintf(stderr, "Invalid port number\n");
                exit(1);
            }
        } else if (strlen(argv[i]) == 2 && strncmp(argv[i], "-v", 2) == 0) {
            *verbose = 1;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            exit(1);
        }
    }
}

// Sets up a server socket on the specified port
int setup_server_socket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        print_error("socket");

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0)
        print_error("bind");

    if (listen(server_fd, 5) < 0)
        print_error("listen");

    return server_fd;
}

// Accepts a client connection
int accept_client(int server_fd) {

    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    int client_fd = accept(server_fd, (struct sockaddr *)&client, &client_len);
    if (client_fd < 0) {
        print_error("accept");
        return -1;
    }

    printf("Client connected: %s:%d\n", inet_ntoa(client.sin_addr),
           ntohs(client.sin_port));
    return client_fd;
}

// Handles communication with the client
void handle_client(int client_fd, int verbose) {
    char buffer[BUFFER_SIZE];
    ssize_t received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) {
        close(client_fd);
        return;
    }

    buffer[received] = '\0';
    if (verbose)
        printf("Request:\n%s\n", buffer);

    HttpRequest req;
    if (!parse_http_request(buffer, &req)) {
        send_http_response(client_fd, "400 Bad Request");
        close(client_fd);
        return;
    }

    if (strncmp(req.method, "GET", 3) != 0 || req.method[3] != '\0') {
        send_http_response(client_fd, "405 Method Not Allowed");
        close(client_fd);
        return;
    }

    if (strncmp(req.path, "/calc/", 6) == 0)
        handle_calc(client_fd, req.path);
    else if (strncmp(req.path, "/static/", 8) == 0)
        serve_static(client_fd, req.path);
    else
        send_404_response(client_fd);

    close(client_fd);
}

// Thread function to handle client connections
void *client_thread(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);
    handle_client(client_fd, 1);
    return NULL;
}
