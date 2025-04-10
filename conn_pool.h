//
// Created by poproshaikin on 10.4.25.
//

#ifndef CONN_POOL_H
#define CONN_POOL_H
#include <netinet/in.h>

typedef struct {
    int id;
    int fd;
    struct sockaddr_in *address;
} Client;

void add_to_pool(int fd, struct sockaddr_in *address);

void send_to_all(const char *message, const size_t len);


#endif //CONN_POOL_H
