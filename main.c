#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "conn_pool.h"
#include "server.h"
#include "utils.h"

#define SERVER_PORT 6969

void listen_loop(struct Server *server) {
    while (1) {
        struct Client *client = accept_client(server);
        if (client->fd < 0) {
            perror("client connection failed");
        }

        // const char *client_ip = get_server_ip(client->fd, server->socket_addr);
        // printf("Connection accepted: %s\n", client_ip);

        struct Message *msg = receive_message(client->fd);
        if (msg->err) {
            perror("message receiving failed");
            continue;
        }
        printf("Message received: %s\n", msg->message);

        if (strcmp(msg->message, "connect") == 0) {
            if (add_to_pool(server->connections_pool, client) != 0) {
                perror("Failed to add client to the pool");
                continue;
            }
        }

        free(msg);
    }
}

void *cli_thread(void *server_p) {
    struct Server *server = server_p;
    while (1) {
        char cmd[128] = {0};
        scanf("%s", cmd);
        if (strcmp(cmd, "stop") == 0) {
            printf("server shutting down\n");
            dispose_server(server);
            exit(0);
        }
        if (strcmp(cmd, "sal") == 0) {
            char buffer[1024] = {0};
            printf("Enter message: ");
            scanf("%s\n", buffer);
            send_to_all(server->connections_pool, buffer, strlen(buffer));
        }
    }
}

int main(void) {
    struct Server server;
    if (create_server(&server, SERVER_PORT, 10) != 0) {
        perror("Failed to start server");
        return 1;
    }
    char *address = get_server_ip(server.socket_fd, server.socket_addr);
    printf("Server started at address %s:%i", address, SERVER_PORT);

    pthread_t pid;
    if (pthread_create(&pid, NULL, cli_thread, &server) != 0) {
        perror("Failed to create cli thread");
        return 1;
    }

    listen_loop(&server);
    dispose_server(&server);

    // free(address);

    return 0;
}
