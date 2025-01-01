#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd1, fd2;
    char *fifo1 = "/tmp/myfifo1";
    char *fifo2 = "/tmp/myfifo2";
    char buffer[128];
    char *replies[] = {
        "Reply 1 from reader process!\n",
        "Reply 2 from reader process!\n",
        "Reply 3 from reader process!\n"
        "END OF MESSAGE\n"
    };

    // Open FIFO1 for read only
    fd1 = open(fifo1, O_RDONLY);
    if (fd1 == -1) {
        perror("open fifo1");
        exit(EXIT_FAILURE);
    }

    // Read the message from FIFO1
    read(fd1, buffer, sizeof(buffer));
    printf("Received: %s", buffer);
    close(fd1);

    // Open FIFO2 for write only
    fd2 = open(fifo2, O_WRONLY);
    if (fd2 == -1) {
        perror("open fifo2");
        exit(EXIT_FAILURE);
    }

    // Write multiple replies to FIFO2
    for (int i = 0; i < 4; i++) {
        printf("Sending: %s", replies[i]);
        write(fd2, replies[i], strlen(replies[i]) + 1);
        usleep(50000); // Sleep for 1 microsecond
    }
    close(fd2);

    return 0;
}