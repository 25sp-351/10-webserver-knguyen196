#ifndef RESPONSE_H
#define RESPONSE_H

void send_http_response(int client_fd, const char *body);
void send_404_response(int client_fd);

#endif  // RESPONSE_H
