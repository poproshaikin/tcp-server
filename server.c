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

static void *listener(void *server_p);
static void *message_observer(void *args_p);

typedef struct {
    struct Server *server;
    struct Client *client;
} MessageListenerThreadArgs;

int create_server(struct Server *server, const int port, const int max_connections) {
    const int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        return -1;
    }

    // ReSharper disable once CppDFAMemoryLeak
    struct sockaddr_in *socket_addr = init_addr(port);

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

    pthread_t new_clients_thread;
    if (pthread_create(&new_clients_thread, NULL, listener, NULL) != 0) {
        return -1;
    }

    pthread_t message_observer_thread;
    if (pthread_create(&message_observer_thread, NULL, message_observer, NULL) != 0) {
        return -1;
    }

    // ReSharper disable once CppDFAMemoryLeak
    server = malloc(sizeof(struct Server));
    server->socket_fd;
    server->socket_addr = socket_addr;
    server->connections_pool = malloc(sizeof(struct Pool));
    server->listening_thread = new_clients_thread;
    pthread_mutex_init(server->mutex, NULL);

    return 0;
}

int on_message(const struct Server *server, const Callback callback) {
    pthread_mutex_lock(server->mutex);

    CallbackOptions **temp = realloc(server->callback_list->collection, (server->callback_list->count + 1) * sizeof(CallbackOptions*));
    if (temp == NULL) {
        return -2;
    }

    CallbackOptions *callback_options = malloc(sizeof(CallbackOptions));
    if (callback_options == NULL) {
        free(temp);
        return -3;
    }
    callback_options->callback = callback;

    server->callback_list->collection = temp;
    server->callback_list->collection[server->callback_list->count + 1] = callback_options;
    server->callback_list->count++;
    pthread_mutex_unlock(server->mutex);

    return 0;
}

// Accepts a socket by a listener SERVER_FD
struct Client *accept_client(struct Server *server) {
    socklen_t len = sizeof(struct sockaddr_in);
    const int client_fd = accept(server->socket_fd , (struct sockaddr*)&server->socket_addr, &len);

    static int id = 0;

    struct Client *client = malloc(sizeof(struct Client));
    client->id = id;
    client->fd = client_fd;
    client->address = server->socket_addr;
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
    message->message = buffer;
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

struct sockaddr_in *init_addr(const int port) {
    // ReSharper disable once CppDFAMemoryLeak
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(port);
    return addr;
}

void dispose_server(struct Server *server) {
    close(server->socket_fd);
    free(server->socket_addr);
    dispose_pool(server->connections_pool);
    free(server);
}

static void *listener(void *server_p) {
    struct Server *server = server_p;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        struct Client *client = accept_client(server);
        struct Message *message = receive_message(client->fd);


        free(message->message);
        free(message);
    }
}

// сделать функцию которая будет запускаться после приема нового клиента и будет слушать его сокет на наличие соединений, после вызовет callback