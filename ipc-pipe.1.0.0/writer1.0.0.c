#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

// create a message writer program that sends a message to a reader program using a FIFO.
// The writer program should then wait for a reply from the reader program.
// The reader program should read the message from the FIFO, display it on the screen, and then send a reply back to the writer program.

// create a buffer with "cmd + data"
// write the buffer to the FIFO

#include <stdint.h>

// Constants
#define BUFFER_SIZE 4096
#define SERVER_IP "127.0.0.1"  // Change this to the server's IP address if needed
#define SERVER_PORT 8090

// Structure to hold the command and socket information
struct cli {
    int mode;
    int sock;
    char cmd[BUFFER_SIZE];
};

#define MSG_BODY_SIZE(x) ((x) * sizeof(int32_t) + sizeof(uint32_t))

/*************************************************/
/*********** command code define               ***/
/*************************************************/

// system command
typedef enum
{
    ORU_CMD_SYSTEM_SHOW_BANNER = 1,
    ORU_CMD_SYSTEM_SHOW_INVENTORY,
    ORU_CMD_SYSTEM_SHOW_SYSTEM_INFO,
    ORU_CMD_SYSTEM_WRITE_MEMORY,
    ORU_CMD_SYSTEM_HOSTNAME,
    ORU_CMD_SYSTEM_NO_HOSTNAME,
    ORU_CMD_SYSTEM_FACTORY_DEFAULTS,
    ORU_CMD_SYSTEM_SAVE_CONFIG,
    ORU_CMD_SYSTEM_RESTORE_CONFIG,
    ORU_CMD_SYSTEM_LOGIN,
    ORU_CMD_SYSTEM_WRITE_DEFAULT,
    ORU_CMD_SYSTEM_SHOW_HOSTNAME,
    ORU_CMD_SYSTEM_CLOCK_SET,
    ORU_CMD_SYSTEM_SHOW_CLOCK,
    ORU_CMD_SYSTEM_CPULOAD_THRESHOLD,
    ORU_CMD_SYSTEM_MEMORY_USAGE_THRESHOLD,
    ORU_CMD_SYSTEM_SHOW_CPULOAD,
    ORU_CMD_SYSTEM_SHOW_MEMORY_USAGE,
    ORU_CMD_SYSTEM_DANGER_TEMPERATURE_THRESHOLD,
    ORU_CMD_SYSTEM_HIGH_HIGH_TEMPERATURE_THRESHOLD,
    ORU_CMD_SYSTEM_HIGH_TEMPERATURE_THRESHOLD,
    ORU_CMD_SYSTEM_LOW_TEMPERATURE_THRESHOLD,
    ORU_CMD_SYSTEM_SHOW_TEMPERATURE,
    ORU_CMD_SYSTEM_SHOW_SYSTEM_SOFTWARE_INFO,
    ORU_CMD_SYSTEM_SHOW_SYSTEM_IMAGE_INFO,
    ORU_CMD_SYSTEM_NUM = ORU_CMD_SYSTEM_SHOW_SYSTEM_IMAGE_INFO
} oru_cmd_system_e;

// sync command
typedef enum {
    ORU_CMD_SYNC_SHOW_PTP_STATUS = 1,
    ORU_CMD_SYNC_SHOW_SYNC_INFO,
    ORU_CMD_SYNC_MODE_PTP,
    ORU_CMD_SYNC_MODE_SYNC_E,
    ORU_CMD_SYNC_MODE_FREERUN,
    ORU_CMD_SYNC_SHOW_SYNCE_STATUS,
    ORU_CMD_SYNC_NUM = ORU_CMD_SYNC_SHOW_SYNCE_STATUS
} oru_cmd_sync_e;

// cplane command
typedef enum {
    ORU_CMD_CPLANE_SHOW_ORAN_ATTEN_STATUS = 1,
    ORU_CMD_CPLANE_SHOW_PWR_STATUS,
    ORU_CMD_CPLANE_SHOW_PORT_INFO,
    ORU_CMD_CPLANE_POWER_AMP,
    ORU_CMD_CPLANE_TX_DPD,
    ORU_CMD_CPLANE_SHOW_ORAN_PRACH_CPLANE_INFO,
    ORU_CMD_CPLANE_NUM = ORU_CMD_CPLANE_SHOW_ORAN_PRACH_CPLANE_INFO
} oru_cmd_cplane_e;

// vlan command
typedef enum {
    ORU_CMD_VLAN_RANGE = 1,
    ORU_CMD_VLAN_SHOW_ALL,
    ORU_CMD_VLAN_SHOW_BRIEF,
    ORU_CMD_VLAN_NUM = ORU_CMD_VLAN_SHOW_BRIEF
} oru_cmd_vlan_e;

// iface command
typedef enum {
    ORU_CMD_IFACE_SHOW_SFP = 1,
    ORU_CMD_IFACE_ORAN_ECPRI_UL_MTU,
    ORU_CMD_IFACE_ORAN_ECPRI_MAC_DESTINATION,
    ORU_CMD_IFACE_SHOW_ORAN_ECPRI_INFO,
    ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_ENABLE,
    ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_DISABLE,
    ORU_CMD_IFACE_ORAN_ECPRI_VLAN_ID,
    ORU_CMD_IFACE_ORAN_PORT_TRUNK_ALLOWED_VLAN_ADD,
    ORU_CMD_IFACE_SHOW_INTERFACE_BRIEF,
    ORU_CMD_IFACE_SHOW_INTERFACE,
    ORU_CMD_IFACE_IP_ADDRESS,
    ORU_CMD_IFACE_NO_IP_ADDRESS,
    ORU_CMD_IFACE_DHCP,
    ORU_CMD_IFACE_NO_DHCP,
    ORU_CMD_IFACE_SHOW_IP_INTERFACE_BRIEF,
    ORU_CMD_IFACE_NUM = ORU_CMD_IFACE_SHOW_IP_INTERFACE_BRIEF
} oru_cmd_iface_e;

// User command
typedef enum {
    ORU_CMD_USER_SHOW_USER = 1,
    ORU_CMD_USER_ADD_USER,
    ORU_CMD_USER_DEL_USER,
    ORU_CMD_USER_CHANGE_PASSWORD,
    ORU_CMD_USER_NUM = ORU_CMD_USER_CHANGE_PASSWORD
} oru_cmd_user_e;

// KPI command
typedef enum {
    ORU_CMD_KPI_SHOW_STATISTICS = 1,
    ORU_CMD_KPI_CLEAR_STATISTICS,
    ORU_CMD_KPI_NUM = ORU_CMD_KPI_CLEAR_STATISTICS
} oru_cmd_kpi_e;

#define MAX_CMD_NUM  ORU_CMD_SYSTEM_NUM

typedef struct {
    char* cmd_name;
    uint32_t func_code;
} cmd_handler_list_t;

typedef enum {
    ORU_MSG_TYPE_SYSTEM= 1,
    ORU_MSG_TYPE_SYNC,
    ORU_MSG_TYPE_CPLANE,
    ORU_MSG_TYPE_VLAN,
    ORU_MSG_TYPE_IFACE,
    ORU_MSG_TYPE_USER,
    ORU_MSG_TYPE_KPI, // Key Performance Indicator
    ORU_MSG_TYPE_NUM
} oru_msg_type_e;

/*************************************************/
/*********** msg define                        ***/
/*************************************************/
typedef struct {
    oru_msg_type_e msg_type;
    uint16_t func_id;
    uint16_t msg_size;
} oru_msg_header_t;

typedef struct {
    uint32_t param_num;
    int32_t param_value[];
} oru_msg_body_t;

typedef struct {
    oru_msg_header_t header;
    oru_msg_body_t body;
} oru_general_msg_t;

typedef struct {
    int32_t error_status;
} oru_general_resp_t;

// System commands msg type

// iface command msg type
typedef struct {
    int32_t error_status;
    char ifacename[32];
} oru_iface_show_interface_req_t;

typedef struct {
    oru_msg_type_e msg_type;
    uint16_t func_id;
    uint16_t msg_size;
} oru_msg_header_t;

typedef struct {
    uint32_t param_num;
    int32_t param_value[];
} oru_msg_body_t;

typedef struct {
    oru_msg_header_t header;
    oru_msg_body_t body;
} oru_general_msg_t;

int buffer_create(char *buffer, oru_general_msg_t* msg)
{
    char *buffer = malloc(msg->header.msg_size + 4);
    if (buffer == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(buffer, "cmd ");

    // Copy the message body to the buffer
    memcpy(buffer + 4, msg, msg->header.msg_size);
}

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