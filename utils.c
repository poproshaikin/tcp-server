//
// Created by poproshaikin on 10.4.25.
//

#include "utils.h"

#include <stddef.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

char *get_ip(const int fd, const struct sockaddr_in *addr) {
    socklen_t addr_len = sizeof(*addr);

    // Get the peer address
    if (getsockname(fd, (struct sockaddr*)addr, &addr_len) == -1) {
        perror("getpeername failed");
        return NULL;
    }

    // Convert the IP address to a string
    return inet_ntoa(addr->sin_addr);
}

char *read_str() {
    const int buffer_chunk = 1024;

    int len = 0;
    int chunks = 1;
    char *buffer = malloc(sizeof(char) * buffer_chunk);
    // добавлять в буффер с изначальной длиной (например 1024)
    // если сообщение больше чем 1024, увеличить буффер на 1024, и записать дальше
    // когда пользователь введет enter, то обрезать длину - len и вернуть указатель
    
    while (1) {
        int chNum= getchar();
        if (chNum == EOF || chNum == '\n') {
            break;
        }
        if (len == chunks * buffer_chunk) {
            chunks++;
            buffer = realloc(buffer, sizeof(char) * buffer_chunk * chunks);
        }
        buffer[len] = (char)chNum;
        len++;
    }

    buffer = realloc(buffer, sizeof(char) * (len + 1));
    buffer[len] = '\0';
    return buffer;
}
