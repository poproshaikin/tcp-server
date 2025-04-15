//
// Created by poproshaikin on 10.4.25.
//

#include "conn_pool.h"
#include "server.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

static void *new_clients_listener(void *server_p);
static void *listen_client(void *args_p);
static pthread_t create_client_listening_thread(struct Server *server, struct Client *client);
static struct sockaddr_in *init_server_addr(const int port);

typedef struct {
    struct Server *server;
    struct Client *client;
} MessageListenerThreadArgs;

int create_server(struct Server *server, const int port, const int max_connections) {
    const int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        return -1;
    }

    struct sockaddr_in *socket_addr = init_server_addr(port);

    if (bind(socket_fd, (struct sockaddr *) socket_addr, sizeof(struct sockaddr_in)) < 0) {
        close(socket_fd);
        free(socket_addr);
        return -2;
    }

    if (listen(socket_fd, max_connections) == -1) {
        close(socket_fd);
        free(socket_addr);
        return -3;
    }

    server->socket_fd = socket_fd;
    server->socket_addr = socket_addr;
    server->connections_pool = create_pool();
    server->callback_list = malloc(sizeof(CallbackList));
    server->callback_list->collection = NULL;
    server->callback_list->count = 0;

    pthread_t new_clients_thread;
    if (pthread_create(&new_clients_thread, NULL, new_clients_listener, server) != 0) {
        return -4;
    }

    server->listening_thread = new_clients_thread;

    server->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(server->mutex, NULL);

    return 0;
}

int on_message(const struct Server *server, Callback callback) {
    pthread_mutex_lock(server->mutex);

    Callback *temp = realloc(server->callback_list->collection, (server->callback_list->count + 1) * sizeof(Callback));
    if (temp == NULL) {
        return -2;
    }

    server->callback_list->collection = temp;
    server->callback_list->collection[server->callback_list->count] = callback;
    server->callback_list->count++;
    pthread_mutex_unlock(server->mutex);

    return 0;
}

struct Client *accept_client(const struct Server *server) {
    struct sockaddr_in *client_addr = malloc(sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    const int client_fd = accept(server->socket_fd , (struct sockaddr*)client_addr, &len);
    if (client_fd == -1) {
        free(client_addr);
        return NULL;
    }

    static int id = 0;
    struct Client *client = malloc(sizeof(struct Client));

    client->id = id++;
    client->fd = client_fd;
    client->address = client_addr;

    return client;
}

// Receives a message from socket CLIENT_FD
struct Message *receive_message(const int client_fd) {
    char buffer[MSG_BUFFER_LEN] = {0};
    const ssize_t received = recv(client_fd, buffer, MSG_BUFFER_LEN, 0);

    if (received == -1) {
        struct Message *error = malloc(sizeof(struct Message));
        error->message = NULL;
        error->len = -1;
        error->err = true;
        return error;
    }
    buffer[received] = '\0';

    struct Message *message = malloc(sizeof(struct Message));
    message->message = strdup(buffer);
    message->len = received;
    message->err = false;
    return message;
}

// Sends a MESSAGE with length LEN to the socket CLIENT_FD
// Returns 0 for success
int send_message(const int client_fd, const char *message, const size_t len) {
    if (send(client_fd, message, len, 0) == -1) {
        return -1;
    }
    return 0;
}

static struct sockaddr_in *init_server_addr(const int port) {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return addr;
}

static struct sockaddr_in *init_client_addr(const int client_fd) {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(client_fd, (struct sockaddr *) addr, &len) == -1) {
        free(addr);
        return NULL;
    }
    return addr;
}

static void *new_clients_listener(void *server_p) {
    struct Server *server = server_p;

    while (1) {
        struct Client *client = accept_client(server);

        if (client == NULL) continue;
        if (client->fd == -1) {
            free(client);
            continue;
        }

        struct Message *message = receive_message(client->fd);
        if (message == NULL || message->err || message->message == NULL) {
            free(message);
            continue;
        }

        if (strcmp(message->message, "connect") == 0) {
            create_client_listening_thread(server, client);
        }

        free(message->message);
        free(message);
    }
}

static pthread_t create_client_listening_thread(struct Server *server, struct Client *client) {
    MessageListenerThreadArgs *args = malloc(sizeof(MessageListenerThreadArgs));
    args->server = server;
    args->client = client;

    pthread_t thread;
    if (pthread_create(&thread, NULL, listen_client, args) != 0) {
        return -1;
    }
    return thread;
}

static void *listen_client(void *args_p) {
    const MessageListenerThreadArgs *args = args_p;
    struct Server *server = args->server;
    struct Client *client = args->client;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        struct Message *message = receive_message(client->fd);
        if (message->err) {
            perror("message receiving failed");
            continue;
        }

        for (int i = 0; i < server->callback_list->count; i++) {
            server->callback_list->collection[i](message, client);
        }

        free(message->message);
        free(message);
    }
}

void dispose_server(struct Server *server) {
    close(server->socket_fd);
    free(server->socket_addr);
    dispose_pool(server->connections_pool);
    pthread_mutex_destroy(server->mutex);
    free(server->mutex);
    free(server);
}

