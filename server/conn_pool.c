//
// Created by poproshaikin on 10.4.25.
//

#include "conn_pool.h"

#include <stdio.h>
#include <stdlib.h>

#include "server.h"

struct Pool *create_pool() {
    struct Pool *pool = malloc(sizeof(struct Pool));
    if (!pool) {
        return NULL;
    }
    pool->collection = NULL;
    pool->count = 0;
    return pool;
}

int add_to_pool(struct Pool *pool, struct Client *client) {
    struct Client **temp = realloc(pool->collection, (pool->count + 1) * sizeof(struct Client*));
    if (!temp) {
        return -2;
    }
    pool->collection = temp;

    pool->collection[pool->count] = client;
    pool->count++;
    return 0;
}

// Sends a message to all clients in the pool
// Returns 0 for success
int send_to_all(const struct Pool *pool, const char *message, const size_t len) {
    for (int i = 0; i < pool->count; i++) {
        const struct Client *client = pool->collection[i];
        if (send_message(client->fd, message, len) != 0) {
            return -1;
        }
    }
    return 0;
}

void dispose_pool(struct Pool *pool) {
    for (int i = 0; i < pool->count; i++) {
        free(pool->collection[i]->address);
        free(pool->collection[i]);
    }
    free(pool);
}