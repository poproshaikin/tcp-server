#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define SERVER_PORT 6969
#define BUFFER_LEN 1024

typedef struct {
    char *message;
    ssize_t len;
    bool err;
} Message;

int server_fd;
struct sockaddr_in *server_addr;

struct sockaddr_in *init_addr() {
    struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(SERVER_PORT);
    return addr;
}

int accept_connection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    return accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
}

// Accepts message from socket CLIENT_FD
// Returns struct with null-terminated string and length or error
Message *receive_message(const int client_fd) {
    char buffer[BUFFER_LEN] = {0};
    const ssize_t received = recv(client_fd, buffer, BUFFER_LEN, 0);

    if (received == -1) {
        Message *error = malloc(sizeof(Message));
        error->message = NULL;
        error->len = -1;
        error->err = true;
        return error;
    }
    buffer[received] = '\0';

    Message *message = malloc(sizeof(Message));
    message->message = buffer;
    message->len = received;
    message->err = false;
    return message;
}

void listen_loop() {
    while (1) {
        const int client_fd = accept_connection();
        if (client_fd < 0) {
            perror("client connection failed");
        }
        printf("Connection accepted: %i\n", client_fd);

        Message *msg = receive_message(client_fd);
        if (msg->err) {
            perror("message receiving failed");
            continue;
        }

        printf("Message received: %s\n", msg->message);

        if (send(client_fd, msg->message, msg->len, 0) == -1) {
            perror("response sending failed");
        }
        free(msg);
    }
}

void *cli_thread() {
    while (1) {
        char buffer[128] = {0};
        scanf("%s", buffer);
        if (strcmp(buffer, "stop") == 0) {
            close(server_fd);
            if (server_addr != NULL)
                free(server_addr);
            exit(0);
        }
    }
}

int main(void) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("socket creation failed\n");
        return 1;
    }
    printf("Socket created\n");

    server_addr = init_addr();

    if (bind(server_fd, (struct sockaddr*)server_addr, sizeof(*server_addr)) < 0) {
        perror("Bind debil field");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
        close(server_fd);
        free(server_addr);
        return 1;
    }

    pthread_t pid;
    if (pthread_create(&pid, NULL, cli_thread, NULL) != 0) {
        perror("failed create thread");
        close(server_fd);
        free(server_addr);
        return 1;
    }
    listen_loop();

    close(server_fd);
    free(server_addr);
    return 0;
}
