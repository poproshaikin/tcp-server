//
// Created by poproshaikin on 10.4.25.
//

#ifndef CONN_POOL_H
#define CONN_POOL_H
#include <netinet/in.h>

struct Pool {
    struct Client **collection;
    int count;
};

struct Pool *create_pool();

int add_to_pool(struct Pool *pool, struct Client *client);

int send_to_all(const struct Pool *pool, const char *message, const size_t len);

void dispose_pool(struct Pool *pool);

#endif //CONN_POOL_H
