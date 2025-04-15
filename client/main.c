//
// Created by poproshaikin on 12.4.25.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "../message.h"
#include "../utils.h"

#define MSG_BUFFER_LEN 2048

typedef struct Client Client;
typedef struct Message Message;

char ip[] = "0.0.0.0";
int port = 26263;

void *receiving_thread(void *client_p) {
    Client *client = client_p;

    // ReSharper disable once CppDFAEndlessLoop
    while (1) {
        const Message *message = receive_message(client);
        if (message->err) {
            perror("message receiving failed");
            continue;
        }
        printf("Message received from %s:%i\n", ip, port);
        printf("\"%s\"", message->message);
    }
}

pthread_t create_receiving_thread(Client *client) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, receiving_thread, client) != 0) {
        perror("failed to create receiving thread");
        exit(1);
    }
    return thread;
}

void *sending_thread(void *client_p) {
    Client *client = client_p;

    while (1) {
        printf("Enter message: ");
        
        char *str = read_str();

        Message message = {
            .message = str,
            .len = strlen(str),
            .err = false
        };
        if (send_message(client, &message) == -1) {
            perror("message sending failed");
            continue;
        }
        printf("Message sent to %s:%i\n", ip, port);
    }
}

pthread_t create_sending_thread(Client *client) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, sending_thread, client) != 0) {
        perror("failed to create listening thread");
        exit(1);
    }
    return thread;
}

int main(int argc, char **argv) {
    /*printf("Enter message: ");*/
    /*char *str = read_str();*/
    /*printf("\n");*/
    /*printf("%s", str);*/
    /*printf("len: %lu", strlen(str));*/

    char *finalIp;
    int finalPort;

    if (argc == 0 && argv == NULL) {
        finalIp = ip;
        finalPort = port;
    } else {
        finalIp = argv[1];
        printf("%s", finalIp);
        finalPort = atoi(argv[2]);
    }

    printf("Creation client with target address: %s:%i\n", finalIp, finalPort);
    Client *client = create_client(ip, port);
    if (client == NULL) {
        perror("client creation failed");
        return -1;
    }

    create_receiving_thread(client);
    create_sending_thread(client);

    while (1) {

    }
}
