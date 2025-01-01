#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>
#include "oru_cli_common.h"

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char req_buffer[BUFFER_SIZE] = {0};
    int bytes_read;
    struct cli *cli = NULL;

    printf("Client connected\n");

    while ((bytes_read = read(client_socket, req_buffer, BUFFER_SIZE)) > 0) {
        if (bytes_read < 0) {
            perror("Read error");
            goto ERR;
        }

        req_buffer[bytes_read] = '\0';

        if (strlen(req_buffer) == 0) {
            printf("Error: Received empty request from client.\n");
            goto ERR;
        }

        cli = (struct cli *)malloc(sizeof(struct cli));
        if (cli == NULL) {
            perror("Memory allocation failed");
            goto ERR;
        }

        memcpy(cli, req_buffer, bytes_read);
        cli->sock = client_socket;

        // Remove newline character
        cli->cmd[strcspn(cli->cmd, "\n")] = 0;

        printf("cli_mode: %d\n", cli->mode);
        printf("received command: %s\n", cli->cmd);

        // Process the command
        command_process(cli);

        free(cli); // Free allocated memory for cli
        cli = NULL;
        printf("Connection closed\n");
        close(client_socket); // Close connection after sending all messages
        memset(req_buffer, 0, BUFFER_SIZE);
        return NULL;
    }

ERR:
    printf("Connection closed\n");
    close(client_socket); // Close connection after sending all messages
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

    // Accept and handle incoming connections
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) >= 0) {
        printf("New connection accepted\n");
        pthread_create(&tid, NULL, handle_client, &client_socket);
        pthread_detach(tid);  // Detach thread to handle multiple clients
    }

    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    close(server_socket);
    return 0;
}