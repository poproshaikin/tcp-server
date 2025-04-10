//
// Created by poproshaikin on 10.4.25.
//

#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>

const char *get_ip(const int client_fd ) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    // Get the peer address
    if (getpeername(client_fd, (struct sockaddr *)&addr, &addr_len) == -1) {
        perror("getpeername failed");
        return NULL;
    }

    // Convert the IP address to a string
    return inet_ntoa(addr.sin_addr);
}
