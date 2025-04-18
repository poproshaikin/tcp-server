//
// Created by poproshaikin on 13.4.25.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <netinet/in.h>
#include "../message.h"

struct Client {
    int server_fd;
    struct sockaddr_in server_addr;
};

struct Client *create_client(const char *ip, const int port);

int send_message(const struct Client *client, const struct Message *message);

struct Message *receive_message(const struct Client *client);

#endif //CLIENT_H
