//
// Created by poproshaikin on 10.4.25.
//

#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>

char *get_server_ip(const int server_fd, const struct sockaddr_in *server_addr);

#endif //UTILS_H
