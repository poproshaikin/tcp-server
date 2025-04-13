//
// Created by poproshaikin on 10.4.25.
//

#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>

char *get_ip(const int server_fd, const struct sockaddr_in *server_addr) {
    socklen_t addr_len = sizeof(*server_addr);

    // Get the peer address
    if (getsockname(server_fd, (struct sockaddr*)server_addr, &addr_len) == -1) {
        perror("getpeername failed");
        return NULL;
    }

    // Convert the IP address to a string
    return inet_ntoa(server_addr->sin_addr);
}
