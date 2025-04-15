//
// Created by poproshaikin on 10.4.25.
//

#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>

char *get_ip(const int fd, const struct sockaddr_in *addr);
char *read_str();

#endif //UTILS_H
