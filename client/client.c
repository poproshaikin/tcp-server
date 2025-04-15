//
// Created by poproshaikin on 13.4.25.
//

#include "client.h"

#include <stdlib.h>
#include "netinet/in.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../message.h"

#define MSG_BUFFER_LEN 2048

static struct sockaddr_in init_server_addr(const char *ip, int port);

struct Client *create_client(const char *ip, const int port) {
    int client_fd= socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket creation failed");
        return NULL;
    }
    struct sockaddr_in server_addr = init_server_addr(ip, port);
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) != 0) {
        close(client_fd);
        return NULL;
    }
    struct Client *client = malloc(sizeof(struct Client));
    if (client == NULL) {
        close(client_fd);
        return NULL;
    }
    client->server_fd = client_fd;
    client->server_addr = server_addr;
    return client;
}

int send_message(const struct Client *client, const struct Message *message) {
    if (send(client->server_fd, message->message, message->len, 0) == -1) {
        return -1;
    }
    return 0;
}

struct Message *receive_message(const struct Client *client) {
    char *buffer = malloc(sizeof(char) * MSG_BUFFER_LEN);
    if (buffer == NULL) {
        return NULL;
    }
    const ssize_t received = recv(client->server_fd, buffer, MSG_BUFFER_LEN, 0);
    if (received == -1) {
        free(buffer);
        return NULL;
    }
    buffer[received] = '\0';
    struct Message *message = malloc(sizeof(struct Message));
    if (message == NULL) {
        free(buffer);
        return NULL;
    }
    message->message = buffer;
    message->len = received;
    message->err = false;
    return message;
}

static struct sockaddr_in init_server_addr(const char *ip, const int port) {
    const struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = inet_addr(ip)
    };
    return addr;
}
