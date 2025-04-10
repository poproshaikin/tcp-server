//
// Created by poproshaikin on 10.4.25.
//

#ifndef CONN_ACTIONS_H
#define CONN_ACTIONS_H

#define MSG_BUFFER_LEN 1024
#include <stdbool.h>
#include <sys/types.h>

typedef struct {
    char *message;
    ssize_t len;
    bool err;
} Message;

int accept_connection(const int server_fd);

// Accepts message from socket CLIENT_FD
// Returns struct with null-terminated string and length or error
Message *receive_message(const int client_fd);

struct sockaddr_in *init_addr(const int server_port);

#endif //CONN_ACTIONS_H
