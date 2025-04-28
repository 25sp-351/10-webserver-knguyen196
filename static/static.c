#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../http/response.h"

// Handles the /static endpoint
void serve_static(int client_fd, const char *path) {
    char filepath[512] = "./static";
    strncat(filepath, path + 7,
            sizeof(filepath) - strlen(filepath) - 1);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        send_404_response(client_fd);
        return;
    }

    struct stat st;
    fstat(fd, &st);
    int size = st.st_size;

    // Determine content type based on file extension
    const char *content_type = "application/octet-stream";
    if (strstr(filepath, ".html"))
        content_type = "text/html";
    else if (strstr(filepath, ".css"))
        content_type = "text/css";
    else if (strstr(filepath, ".js"))
        content_type = "application/javascript";
    else if (strstr(filepath, ".png"))
        content_type = "image/png";
    else if (strstr(filepath, ".jpg") || strstr(filepath, ".jpeg"))
        content_type = "image/jpeg";
    else if (strstr(filepath, ".gif"))
        content_type = "image/gif";

    // Send HTTP header
    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n",
             content_type, size);
    write(client_fd, header, strlen(header));

    // Send file content
    char buffer[1024];
    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0)
        write(client_fd, buffer, n);

    close(fd);
}
