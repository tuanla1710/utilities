#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#include "oru_cli_common.h"

// Constants
#define PIPE_NAME "/tmp/oam_pipe"
#define PIPE_NAME_RESP "/tmp/oam_pipe_resp"
#define BUFFER_SIZE 4096
#define LOG_FILE "/tmp/cli_out.txt"
#define REPLY_COUNT 4

void log_error(const char *message) {
    perror(message);  // Log the error with a description of the problem
}

bool create_fifo_if_needed(const char *fifo_name) {
    if (access(fifo_name, F_OK) == -1) {
        if (mkfifo(fifo_name, 0666) == -1) {
            log_error("mkfifo failed");
            return false;
        }
    }
    return true; 
}

ssize_t read_from_fifo(int fd, char *buffer, size_t size) {
    ssize_t bytesRead = read(fd, buffer, size);
    if (bytesRead == -1) {
        log_error("read failed");
        return -1;  
    }
    return bytesRead;
}

bool write_to_fifo(int fd, const char *message) {
    if (write(fd, message, strlen(message) + 1) == -1) {
        log_error("write failed");
        return false;
    }
    return true;
}

void write_to_log(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (!file) {
        log_error("fopen failed");
        return;
    }

    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // fprintf(file, "[%s] %s", time_str, message);
    fprintf(file, "%s", message);
    fclose(file);
}

// CLI output function
void cli_out(struct cli *cli, const char *format, ...) {
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    
    write(cli->sock, buffer, strlen(buffer) + 1);
    usleep(1); 
    // write_to_log(buffer);  // Optional log to file
}

// Dump received data in hexadecimal format
void dump_data(const char *buffer, size_t length) {
    printf("Data dump:\n");
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

// Unpack the request data from the buffer
bool unpack_data_from_buffer(const char *buffer, const char *key, oru_general_msg_t *req) {
    if (!buffer || !key || !req) {
        return false;
    }

    uint16_t cmd_len = strlen(key);
    if (strncmp(buffer, key, cmd_len) != 0) {
        return false;
    }

    const char *req_data = buffer + cmd_len;
    const oru_msg_header_t *req_header = (const oru_msg_header_t *)req_data;

    if (req_header->msg_size > BUFFER_SIZE - cmd_len) {
        printf("Debug: Invalid message size.\n");
        return false;
    }

    memcpy(req, req_data, req_header->msg_size);
    return true;
}

// Display file content by sending each line to the FIFO
void display_file_content(int fd2, const char *file_path) {
    struct cli cli = {0};
    cli.sock = fd2;

    FILE *file = fopen(file_path, "r");
    if (!file) {
        log_error("fopen failed");
        return;  // Just return, don't exit on error
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        cli_out(&cli, line);
    }
    fclose(file);
}

// Function to handle a request
bool oam_handle_cli_request(const char *buffer, const char *fifo2, oru_general_msg_t *req) {
    int fd2;
    char *replies[] = {
        "Reply 1 from reader process!\n",
        "Reply 2 from reader process!\n",
        "Reply 3 from reader process!\n",
        "Reply 4 from reader process!\n"
    };
    char *endofmessage = "END OF MESSAGE\n";
    char *cmd = "cmd ";
    
    fd2 = open(fifo2, O_WRONLY);
    if (fd2 == -1) {
        log_error("open fifo2 failed");
        return false;
    }

    if (unpack_data_from_buffer(buffer, cmd, req)) {
        printf("Unpacked data successfully.\n");
        printf("Msg type: %d\n", req->header.msg_size);
        printf("Function ID: %d\n", req->header.func_id);
        printf("Message size: %d\n", req->header.msg_size);
    } else {
        printf("Failed to unpack data.\n");
        close(fd2);
        return false;
    }

    // If it's a debug command, send replies
    if (req->header.msg_type == ORU_MSG_TYPE_DEBUG && req->header.func_id == ORU_CMD_DEBUG_CMD_TEST) {
        printf("Received debug command test.\n");

        // Write multiple replies to FIFO2
        for (int i = 0; i < REPLY_COUNT; i++) {
            printf("Sending: %s", replies[i]);
            if (!write_to_fifo(fd2, replies[i])) {
                close(fd2);
                return false; 
            }
            usleep(1000);
        }

        // Write file content to FIFO2
        const char *file_path = "/home/user/projects/branches/master/pc805-mplane/sources/oam_netconf_1.0.0/usecases/pc805/uc2/du_config_req.json";
        display_file_content(fd2, file_path);
    }

    // Write the end of message to FIFO2
    if (!write_to_fifo(fd2, endofmessage)) {
        close(fd2);
        return false; 
    }

    close(fd2);
    return true;
}

int main() {
    int fd1;
    char buffer[BUFFER_SIZE];
    
    // Define FIFO names
    char *fifo1 = PIPE_NAME;
    char *fifo2 = PIPE_NAME_RESP;
    
    // Ensure FIFOs exist
    if (!create_fifo_if_needed(fifo1) || !create_fifo_if_needed(fifo2)) {
        return -1;
    }

    // Open FIFO1 for read only
    fd1 = open(fifo1, O_RDONLY);
    if (fd1 == -1) {
        log_error("open fifo1 failed");
        return -1;
    }

    // Read the message from FIFO1
    ssize_t bytesRead = read_from_fifo(fd1, buffer, sizeof(buffer));
    if (bytesRead == -1) {
        close(fd1);
        return -1;
    }

    printf("Received: %s\n", buffer);
    printf("Number of bytes received: %zd\n", bytesRead);

    close(fd1);

    // Initialize request structure
    char buffer2[BUFFER_SIZE] = {0};
    oru_general_msg_t *req = (oru_general_msg_t *)&buffer2[0];

    // Call the handle_request function with the read buffer
    if (!oam_handle_cli_request(buffer, fifo2, req)) {
        return -1;  // Return failure if handling the request fails
    }

    return 0;
}