//
// Created by poproshaikin on 10.4.25.
//

#include "conn_actions.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

int accept_connection(const int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
}

Message *receive_message(const int client_fd) {
    char buffer[MSG_BUFFER_LEN] = {0};
    const ssize_t received = recv(client_fd, buffer, MSG_BUFFER_LEN, 0);

    if (received == -1) {
        Message *error = malloc(sizeof(Message));
        error->message = NULL;
        error->len = -1;
        error->err = true;
        return error;
    }
    buffer[received] = '\0';

    Message *message = malloc(sizeof(Message));
    message->message = buffer;
    message->len = received;
    message->err = false;
    return message;
}

struct sockaddr_in *init_addr(const int port) {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return addr;
}
