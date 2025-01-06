#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

// Constants
#define BUFFER_SIZE 4096
#define SERVER_IP "127.0.0.1"  // Change this to the server's IP address if needed
#define SERVER_PORT 8090

// CLI modes match the command codes at server
typedef enum {
    ORU_MSG_TYPE_SYSTEM= 1,
    ORU_MSG_TYPE_SYNC,
    ORU_MSG_TYPE_CPLANE,
    ORU_MSG_TYPE_VLAN,
    ORU_MSG_TYPE_IFACE,
    ORU_MSG_TYPE_USER,
    ORU_MSG_TYPE_KPI, // Key Performance Indicator
    ORU_MSG_TYPE_DEBUG, 
    ORU_MSG_TYPE_NUM
} oru_msg_type_e;

oru_msg_type_e g_mode = ORU_MSG_TYPE_NUM;

// Structure to hold the command and socket information
struct cli {
    int mode;
    int sock;
    char cmd[BUFFER_SIZE];
};

typedef struct {
    const char *cmd_prefix;
    size_t prefix_len;
    int mode_type;
    const char *mode_string;
} mode_entry_t;


void print_help_mode() {
    printf("Available modes:\n");
    printf("  system\n");
    printf("  sync\n");
    printf("  cplane\n");
    printf("  vlan\n");
    printf("  iface\n");
    printf("  user\n");
    printf("  kpi\n");
    printf("  debug\n");
    printf("  help\n");
}

void print_help_system() {
    printf("oru-system commands:\n");
    printf("  show banner\n");
    printf("  show inventory\n");
    printf("  show system info\n");
    printf("  write memory\n");
    printf("  hostname NAME\n");
    printf("  no hostname NAME\n");
    printf("  factory-defaults\n");
    printf("  save-config\n");
    printf("  restore-config\n");
    printf("  login\n");
    printf("  write default\n");
    printf("  write memory\n");
    printf("  show hostname\n");
    printf("  clock set HH:MM:SS <1-31> <1-12> <2000-2099>\n");
    printf("  show clock\n");
    printf("  cpuload threshold <0-100>\n");
    printf("  memory-usage threshold <0-100>\n");
    printf("  show cpuload\n");
    printf("  show memory-usage\n");
    printf("  danger termperature threshold <90-100>\n");
    printf("  high-high termperature threshold <80-90>\n");
    printf("  high termperature threshold<0-80>\n");
    printf("  low termperature threshold<-40-30>\n");
    printf("  show temperature\n");
    printf("  show system software info\n");
    printf("  show system image info\n");
    printf("  show sysrepo info\n");
    printf("  exit\n");
    printf("  help\n");
}

void print_help_sync() {
    printf("oru-sync commands:\n");
    printf("  show (ptp|synce) status\n");
    printf("  show sync info\n");
    printf("  sync mode (ptp|synce|freerun)\n");
    printf("  exit\n");
    printf("  help\n");
}

void print_help_cplane() {
    printf("oru-cplane commands:\n");
    printf("  show oran atten status\n");
    printf("  show pwr status\n");
    printf("  show port info\n");
    printf("  power-amp [enable | disable ] e-axcid <1-16> afe 0 ch [0 | 1 | 2 | 3]\n"); 
    printf("  tx-dpd [reset | apply ] e-axcid <1-16> afe 0 ch <0-3>\n");
    printf("  show oran prach cplane info\n");
    printf("  exit\n");
    printf("  help\n");
}

void print_help_vlan() {
    printf("oru-vlan commands:\n");
    printf("  vlan range <1-4096>\n");
    printf("  show vlan all\n");
    printf("  show vlan brief\n");
    printf("  exit\n");
    printf("  help\n");    
}

void print_help_iface() {
    printf("oru-iface commands:\n");
    printf("  show sfp\n");
    printf("  oran ecpri ul mtu <64-9000>\n");
    printf("  oran ecpri mac destination MACADDR\n");
    printf("  show oran ecpri info\n");
    printf("  oran ecpri vlan control enable\n");
    printf("  oran ecpri vlan control disable\n");
    printf("  oran ecpri vlan id <0-4094>\n");
    printf("  oran port trunk allowed vlan add VLAN_LIST interface <IFNAME>\n");
    printf("  show interface brief\n");
    printf("  show interface IFNAME\n");
    printf("  ip address <A.B.C.D>/<M> interface <IFNAME>\n");
    printf("  no ip address <A.B.C.D>/<M> interface <IFNAME>\n");
    printf("  dhcp interface <IFNAME>\n");
    printf("  no dhcp interface <IFNAME>\n");
    printf("  show ip interface brief\n");
    printf("  exit\n");
    printf("  help\n");    
}

void print_help_user() {
    printf("oru-user commands:\n");
    printf("  show user\n");
    printf("  add username USERNAME role WORD password WORD\n");
    printf("  del username USERNAME\n");
    printf("  change password USERNAME password WORD\n");
    printf("  exit\n");
    printf("  help\n");
}

void print_help_kpi() {
    printf("oru-kpi commands:\n");
    printf("  show oran kpi statistics\n");
    printf("  clear oran kpi statistics\n");
    printf("  exit\n");
    printf("  help\n");
}

void print_help_debug() {
    printf("oru-debug commands:\n");
    printf("  cmd test\n");
    printf("  exit\n");
    printf("  help\n");    
}

void print_help_all() {
    print_help_system();
    print_help_sync();
    print_help_cplane();
    print_help_vlan();
    print_help_iface();
    print_help_user();
    print_help_kpi();
    print_help_debug();

    printf("\n");
    print_help_mode();
}


void print_help(oru_msg_type_e mode)
{
    switch (mode) {
        case ORU_MSG_TYPE_SYSTEM:
            print_help_system();
            break;
        case ORU_MSG_TYPE_SYNC:
            print_help_sync();
            break;
        case ORU_MSG_TYPE_CPLANE:
            print_help_cplane();
            break;
        case ORU_MSG_TYPE_VLAN:
            print_help_vlan();
            break;
        case ORU_MSG_TYPE_IFACE:
            print_help_iface();
            break;
        case ORU_MSG_TYPE_USER:
            print_help_user();
            break;
        case ORU_MSG_TYPE_KPI:
            print_help_kpi();
            break;
        case ORU_MSG_TYPE_DEBUG:
            print_help_debug();
            break;
        case ORU_MSG_TYPE_NUM:
            print_help_mode();
            break;
        default:
            print_help_all();
            break;
    }
}

void print_and_clear_log(const char *log_file) {
    FILE *file = fopen(log_file, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    // Print all lines in the file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    // Clear all content in the file
    file = fopen(log_file, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    fclose(file);
}

void check_and_create_file(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        if (errno == ENOENT)
        {
            // File does not exist, create it
            file = fopen(file_path, "w");
            if (file == NULL)
            {
                perror("fopen");
                return;
            }
            fclose(file);
        }
        else
        {
            perror("fopen");
            return;
        }
    }
}

void send_command(struct cli *cli) {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        goto ERR;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        goto ERR;
    }

    // Calculate the size of the data to send
    size_t data_size = sizeof(cli->mode) + sizeof(cli->sock) + strlen(cli->cmd) + 1;

    // Send the command to the server
    if (send(sock, (void*)cli, data_size, 0) < 0) {
        perror("Send failed");
        goto ERR;
    }

    // Receive the response from the server
    char buffer[BUFFER_SIZE] = {0};
    int bytes_read = read(sock, buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("%s\n", buffer);
    } else {
        if (bytes_read < 0) {
            perror("Read error");
        }
    }

    // open file and print all line in the file. Then clear all content in /tmp/mylogfile.log
    print_and_clear_log("/tmp/cli_out.txt");



ERR:

    close(sock);
}

char *get_mode(char *cmd) {
    static const mode_entry_t mode_table[] = {
        {"sync", 4, ORU_MSG_TYPE_SYNC, "oru-sync"},
        {"vlan", 4, ORU_MSG_TYPE_VLAN, "oru-vlan"},
        {"cplane", 6, ORU_MSG_TYPE_CPLANE, "oru-cplane"},
        {"iface", 4, ORU_MSG_TYPE_IFACE, "oru-iface"},
        {"user", 4, ORU_MSG_TYPE_USER, "oru-user"},
        {"kpi", 3, ORU_MSG_TYPE_KPI, "oru-kpi"},
        {"system", 6, ORU_MSG_TYPE_SYSTEM, "oru-system"},
        {"debug", 6, ORU_MSG_TYPE_DEBUG, "oru-debug"}
    };

    for (size_t i = 0; i < sizeof(mode_table) / sizeof(mode_table[0]); i++) {
        if (strncmp(cmd, mode_table[i].cmd_prefix, mode_table[i].prefix_len) == 0) {
            g_mode = mode_table[i].mode_type;
            return (char *)mode_table[i].mode_string;
        }
    }

    g_mode = ORU_MSG_TYPE_NUM;
    return "invalid mode";
}

int main(int argc, char *argv[]) {
    char hostname[256];
    struct cli cli; 
    
    memset(&cli, 0, sizeof(struct cli));

    check_and_create_file("/tmp/cli_out.txt");

    // Get the hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        return 1;
    }

    // Get the CLI mode
    char g_mode_str[256];
    if (argc > 1) {
        strcpy(g_mode_str, get_mode(argv[1]));
    } else {
        printf("invalid cli mode\n");
        print_help_mode();
        return 1;
    }
    
    if (g_mode == ORU_MSG_TYPE_NUM)
    {
        print_help_all();
        return 1;
    }

    cli.mode = g_mode;

    printf("Type 'help' to see the list of commands.\n");

    while (1) {
        // clear cli.cmd buffer
        memset(cli.cmd, 0, sizeof(cli.cmd));

        printf("%s(%s)# ", hostname, g_mode_str);  // Modified prompt with hostname
        if (fgets(cli.cmd, sizeof(cli.cmd), stdin) == NULL) {
            break;
        }

        // Remove newline character
        cli.cmd[strcspn(cli.cmd, "\n")] = 0;

        // Remove trailing spaces
        char *end = cli.cmd + strlen(cli.cmd) - 1;
        while (end > cli.cmd && *end == ' ') {
            *end = '\0';
            end--;
        }
        // printf("Debug: Command entered: '%s'\n", cli.cmd);

        if (strcmp(cli.cmd, "exit") == 0) {
            printf("Exiting...\n");
            break;
        } else if (strcmp(cli.cmd, "help") == 0 || strcmp(cli.cmd, "?") == 0 || strcmp(cli.cmd, "ls") == 0) {
            print_help(g_mode);
        } else if (strlen(cli.cmd) == 0) {
            // Ignore empty command
            continue;
        } else {
            send_command(&cli);
        }
    }

    return 0;
}