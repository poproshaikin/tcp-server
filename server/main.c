#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>

#include "conn_pool.h"
#include "server.h"
#include "../utils.h"

#define SERVER_PORT 26263

void listen_loop(struct Server *server) {
    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        struct Client *client = accept_client(server);
        if (client->fd < 0) {
            perror("client connection failed");
            free(client);
            continue;
        }

        printf("Connection accepted: %s\n", get_ip(client->fd, client->address));

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
    printf("Server CLI started\n");
    printf("Enter commands to the console to control the server\n");
    while (1) {
        char *cmd = read_str();
        if (strcmp(cmd, "stop") == 0) {
            printf("Server shutting down\n");
            dispose_server(server);
            exit(0);
        }
        else if (strcmp(cmd, "sal") == 0) {
            printf("Enter message: ");
            char *msg = read_str();
            send_to_all(server->connections_pool, msg, strlen(msg));
        }
        else {

        }
    }
}

int main(int argc, char **argv) {
    int finalPort = SERVER_PORT;
    if (argc == 0 && argv == NULL) {
        finalPort = atoi(argv[1]);
    }

    struct Server server;
    if (create_server(&server, finalPort, 10) != 0) {
        perror("Failed to start server");
        return 1;
    }
    char *address = get_ip(server.socket_fd, server.socket_addr);
    printf("Server started at address %s:%i\n", address, SERVER_PORT);

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
