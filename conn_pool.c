//
// Created by poproshaikin on 10.4.25.
//

#include "conn_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"

static Client **collection = NULL;
static int collection_size = 0;

void add_to_pool(const int fd, struct sockaddr_in *address) {
    Client *new_client = malloc(sizeof(Client));
    if (!new_client) {
        perror("Failed to allocate memory for new client");
        return;
    }

    new_client->id = collection_size;
    new_client->fd = fd;
    new_client->address = address;

    Client **temp = realloc(collection, (collection_size + 1) * sizeof(Client*));
    if (!temp) {
        perror("Failed to resize client collection");
        free(new_client);
        return;
    }
    collection = temp;

    collection[collection_size] = new_client;
    collection_size++;
}

void send_to_all(const char *message, const size_t len) {
    for (int i = 0; i < collection_size; i++) {
        const Client *client = collection[i];
        send_message(client->fd, message, len);
    }
}