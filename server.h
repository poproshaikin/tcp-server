//
// Created by poproshaikin on 10.4.25.
//

#ifndef CONN_ACTIONS_H
#define CONN_ACTIONS_H

#define MSG_BUFFER_LEN 2048
#include <stdbool.h>
#include <sys/types.h>
#include <netinet/in.h>

struct Message {
    char *message;
    ssize_t len;
    bool err;
};

struct Client {
    int id;
    int fd;
    struct sockaddr_in *address;
};

typedef void(*Callback)(struct Message *, struct Client *);

typedef struct {
    Callback callback;
} CallbackOptions;

typedef struct {
    CallbackOptions **collection;
    int count;
} CallbackList;

struct Server {
    int socket_fd;
    struct sockaddr_in *socket_addr;
    struct Pool *connections_pool;
    CallbackList *callback_list;

    pthread_t listening_thread;
    pthread_mutex_t *mutex;
};

int create_server(struct Server *server, int port, int max_connections);

int on_message(const struct Server *server, const Callback callback);

struct Client *accept_client(struct Server *server);

// Accepts message from socket CLIENT_FD
// Returns struct with null-terminated string and length or error
struct Message *receive_message(int client_fd);

int send_message(int client_fd, const char *message, size_t len);

struct sockaddr_in *init_addr(int port);

void dispose_server(struct Server *server);

#endif //CONN_ACTIONS_H
