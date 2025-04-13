//
// Created by poproshaikin on 13.4.25.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>
#include <sys/types.h>

struct Message {
    char *message;
    ssize_t len;
    bool err;
};

#endif //MESSAGE_H
