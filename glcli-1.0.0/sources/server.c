#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "oru_cli_common.h"

int handle_req_to_oam(struct cli* cli, oru_general_msg_t* req) {
    int fd1, fd2;
    char buffer[BUFFER_SIZE] = {0};
    char buffer2[BUFFER_SIZE] = {0};
    const char *cmd = "cmd ";

    if (access(PIPE_NAME, F_OK) == -1 && mkfifo(PIPE_NAME, 0666) == -1) {
        perror("mkfifo fifo1");
        return -1;
    }

    if (access(PIPE_NAME_RESP, F_OK) == -1 && mkfifo(PIPE_NAME_RESP, 0666) == -1) {
        perror("mkfifo fifo2");
        return -1;
    }

    fd1 = open(PIPE_NAME, O_WRONLY);
    if (fd1 == -1) {
        perror("open fifo1");
        return -1;
    }

    uint16_t offset = strlen(cmd);
    memcpy(buffer2, cmd, offset);

    if (offset + req->header.msg_size > BUFFER_SIZE) {
        printf("Error: Message size exceeds buffer size\n");
        close(fd1);
        return -1;
    }

    memcpy(buffer2 + offset, req, req->header.msg_size);
    if (write(fd1, buffer2, offset + req->header.msg_size) == -1) {
        perror("write to fifo1");
        close(fd1);
        return -1;
    }

    close(fd1);

    fd2 = open(PIPE_NAME_RESP, O_RDONLY);
    if (fd2 == -1) {
        perror("open fifo2");
        return -1;
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd2, &readfds);

    struct timeval start_time, current_time;
    gettimeofday(&start_time, NULL);

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        struct timeval timeout = {3, 0};
        int retrial = select(fd2 + 1, &readfds, NULL, NULL, &timeout);
        if (retrial == -1) {
            perror("select");
            close(fd2);
            return -1;
        } else if (retrial == 0) {
            printf("No data within three seconds.\n");
            break;
        }

        int bytesRead = read(fd2, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            if (strcmp(buffer, "END OF MESSAGE\n") == 0) {
                cli_out(cli, "\n");
                break;
            }
            cli_out(cli, "%s", buffer);
        }

        gettimeofday(&current_time, NULL);
        long elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000000 + (current_time.tv_usec - start_time.tv_usec);
        if (elapsed_time >= 3000000) {
            printf("Debug: Overall timeout of 3 seconds reached.\n");
            break;
        }
    }

    close(fd2);
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char req_buffer[BUFFER_SIZE] = {0};
    int bytes_read;

    printf("Client connected\n");

    while ((bytes_read = read(client_socket, req_buffer, BUFFER_SIZE)) > 0) {
        if (bytes_read < 0) {
            perror("Read error");
            break;
        }

        req_buffer[bytes_read] = '\0';

        if (strlen(req_buffer) == 0) {
            printf("Error: Received empty request from client.\n");
            break;
        }

        struct cli *cli = (struct cli *)malloc(sizeof(struct cli));
        if (cli == NULL) {
            perror("Memory allocation failed");
            break;
        }

        memcpy(cli, req_buffer, bytes_read);
        cli->sock = client_socket;

        cli->cmd[strcspn(cli->cmd, "\n")] = 0;

        printf("cli_mode: %d\n", cli->mode);
        printf("received command: %s\n", cli->cmd);

        command_process(cli);

        free(cli);
        close(client_socket);
        printf("Connection closed\n");
        return NULL;
    }

    close(client_socket);
    return NULL;
}

int main(void) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) >= 0) {
        printf("New connection accepted\n");
        pthread_create(&tid, NULL, handle_client, &client_socket);
        pthread_detach(tid);
    }

    if (client_socket < 0) {
        perror("Accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    close(server_socket);
    return 0;
}