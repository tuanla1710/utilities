#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

int main() {
    int fd1, fd2;
    char *fifo1 = "/tmp/myfifo1";
    char *fifo2 = "/tmp/myfifo2";
    char *message = "Hello from writer process!\n";
    char buffer[128];

    // Open FIFO1 for write only
    fd1 = open(fifo1, O_WRONLY);
    if (fd1 == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    // Write the message to FIFO1
    write(fd1, message, strlen(message) + 1);
    close(fd1);

    // Open FIFO2 for read only
    fd2 = open(fifo2, O_RDONLY);
    if (fd2 == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    // Set up the file descriptor set
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd2, &readfds);

    // Get the start time
    struct timeval start_time, current_time;
    gettimeofday(&start_time, NULL);

    // Read multiple replies from FIFO2 until "END OF MESSAGE" is received
    while (1)
    {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer

        // wait for data to be available for reading with a timeout of 3 seconds
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;

        // Set up the file descriptor set
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd2, &readfds);

        // wait for data to be available for reading with a timeout of 3 secondds
        // printf("Debug: Waiting for data with a timeout of 3 seconds...\n");
        int retral = select(fd2 + 1, &readfds, NULL, NULL, &timeout);
        if (retral == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (retral == 0)
        {
            printf("No data within three seconds.\n");
            break;
        }

        int bytesRead = read(fd2, buffer, sizeof(buffer) - 1); // Read into buffer
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            printf("Debug: Received: %s", buffer);
            if (strcmp(buffer, "END OF MESSAGE\n") == 0)
            {
                printf("Debug: End of message received!\n");
                break;
            }
            else if (bytesRead == 0)
            {
                printf("Debug: End of file.\n");
                break;
            } else {
                perror("read");
            }
        }

        // Check if the overall timeout has been reached
        gettimeofday(&current_time, NULL);
        long elapsed_time = (current_time.tv_sec - start_time.tv_sec) * 1000000 + (current_time.tv_usec - start_time.tv_usec);
        if (elapsed_time >= 3000000) { // 3 seconds in microseconds
            printf("Debug: Overall timeout of 3 seconds reached.\n");
            break;
        }
    }

    printf("End of message received!\n");
    close(fd2);

    return 0;
}