/*
 * GIGALANE PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with GIGALANE and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright GIGALANE.
 */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "oru_cli_common.h"
#include "oru_cli_common_lib.h"

// show mplane info
#define MAX_LINE_LENGTH 256
#define MAX_TABLE_ROWS 50


int validate_and_convert_iface(char *iface) {
    // Check if the interface is valid
    if (strcmp(iface, "mgmt") != 0 && strcmp(iface, "eth0") != 0 && strcmp(iface, "enp0s3") != 0) {
        printf("Error - Invalid interface: %s\n", iface);
        return -1; // Error
    }

    // Convert "mgmt" to "eth0"
    if (strcmp(iface, "mgmt") == 0) {
        strncpy(iface, "eth0", IFNAMSIZ - 1);
        iface[IFNAMSIZ - 1] = '\0'; // Ensure null-termination
    }

    return 0; // Success
}

bool show_interface_info(const char* ifname, char* output, size_t output_len) {
    if (ifname == NULL || output == NULL) {
        return false;
    }

    char cmd[MAX_IFNAME_LEN + 10]; // "ifconfig " + ifname + null terminator
    snprintf(cmd, sizeof(cmd), "ifconfig %s", ifname);

    FILE* pipe = popen(cmd, "r");
    if (pipe == NULL) {
        fprintf(stderr, "popen failed: %s\n", strerror(errno));
        return false;
    }

    size_t total_read = 0;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t len = strlen(buffer);
        if (total_read + len < output_len - 1) {
            strcpy(output + total_read, buffer);
            total_read += len;
        } else {
            break;
        }
    }

    if (ferror(pipe)) {
        fprintf(stderr, "fgets failed: %s\n", strerror(errno));
        pclose(pipe);
        return false;
    }

    pclose(pipe);
    return true;
}

int remove_ip_address(const char *iface, const char *ip, int mask)
{
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_in *addr;

    // Print debug info
    printf("Function: %s, Line: %d\n", __func__, __LINE__);
    printf("Input Parameters - Interface: %s, IP Address: %s, Mask: %d\n", iface, ip, mask);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    // Prepare the ifreq structure
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    // Prepare the sockaddr_in structure
    addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr->sin_addr);
    addr->sin_port = 0;

    // Perform the ioctl operation to remove the IP address
    if (ioctl(sockfd, SIOCDIFADDR, &ifr) < 0)
    {
        perror("ioctl");
        close(sockfd);
        return -1;
    }

    // Close the socket
    close(sockfd);
    return 0;
}

#define MAX_LINE_LENGTH 256
void print_table_header(struct cli *cli) {
    cli_out(cli, "| %-8s | %-4s | %-15s | %-17s | %-5s |\n", "ifname", "state", "ipv4 addr", "mac", "mtu");
    cli_out(cli, "|%-10s|%-6s|%-17s|%-19s|%-7s|\n", "----------", "------", "-----------------", "-------------------", "-----");
}

void print_table_row(struct cli *cli, const char *ifname, const char *state, const char *ipv4, const char *mac, const char *mtu) {
    cli_out(cli, "| %-8s | %-4s | %-15s | %-17s | %-5s |\n", ifname, state, ipv4, mac, mtu);
}

void show_ip_interface_brief(struct cli *cli) {
    FILE* fp;
    char path[1035];

    // Open the command for reading
    fp = popen("ip addr", "r");
    if (fp == NULL) {
        cli_out(cli, "Failed to run command\n");
        exit(1);
    }

    // Read the output a line at a time - output it
    while (fgets(path, sizeof(path), fp) != NULL) {
        cli_out(cli, "%s", path);
    }

    // Close the file pointer
    pclose(fp);
}

void display_system_clock(struct cli *cli)
{
    FILE *fp;
    char path[1035];

    // Open the command for reading
    fp = popen("date", "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        return;
    }

    // Read the output a line at a time and print it
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        cli_out(cli, "%s", path);
    }

    // Close the file pointer
    pclose(fp);
}

void display_system_cpu_load(struct cli* cli) {
    FILE *fp;
    char buffer[1024];
    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long int total1, total2, idle1, idle2;

    // Read the first line of /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/stat\n");
        return;
    }
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        printf("Failed to read /proc/stat\n");
        fclose(fp);
        return;
    }
    sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(fp);

    // Calculate total and idle times
    total1 = user + nice + system + idle + iowait + irq + softirq + steal;
    idle1 = idle + iowait;

    // Sleep for a second
    sleep(1);

    // Read the first line of /proc/stat again
    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/stat\n");
        return;
    }
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        printf("Failed to read /proc/stat\n");
        fclose(fp);
        return;
    }
    sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    fclose(fp);

    // Calculate total and idle times again
    total2 = user + nice + system + idle + iowait + irq + softirq + steal;
    idle2 = idle + iowait;

    // Calculate CPU load
    double total_diff = total2 - total1;
    double idle_diff = idle2 - idle1;
    double cpu_load = (total_diff - idle_diff) / total_diff * 100.0;

    // Display CPU load
    cli_out(cli, "Total CPU Load: %.2f%%\n", cpu_load);
}

void show_memory_usage(struct cli* cli)
{
    FILE *fp;
    char buffer[256];

    // Open the /proc/meminfo file for reading
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/meminfo\n");
        return;
    }

    // Read and print each line of the file
    cli_out(cli, "Memory Usage:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        cli_out(cli, "%s", buffer);
    }

    // Close the file pointer
    fclose(fp);
}

// DATABSE HANDLE

// Update the IP address to the database
int update_ip_address(const char *iface, const char *ip, int mask)
{
    // Print debug info
    printf("Function: %s, Line: %d\n", __func__, __LINE__);
    printf("Input Parameters - Interface: %s, IP Address: %s, Mask: %d\n", iface, ip, mask);

    // Update the IP address to the database
    // (Add your database update logic here)

    return 0;    
}

bool db_check_ip_address(const char *iface, const char *ip, int mask) {
    // Print debug info
    printf("Function: %s, Line: %d\n", __func__, __LINE__);
    printf("Input Parameters - Interface: %s, IP Address: %s, Mask: %d\n", iface, ip, mask);

    // Mock database check logic
    return true;
}

int check_ip_address(const char *iface, const char *ip, int mask) {
    if (db_check_ip_address(iface, ip, mask)) {
        return 0;
    } else {
        return -1;
    }
}

int get_static_ip(const char *iface, char *ip, char *mask, char *gateway, char *dns1, char *dns2) {
    printf("Input Parameters - Interface: %s\n", iface);

    // Mock static IP retrieval logic from eepprom. FIXME
    strncpy(ip, "192.168.0.100", INET_ADDRSTRLEN);
    strncpy(mask, "255.255.255.0", INET_ADDRSTRLEN);
    strncpy(gateway, "192.168.0.1", INET_ADDRSTRLEN);
    strncpy(dns1, "8.8.8.8", INET_ADDRSTRLEN);
    strncpy(dns2, "8.8.4.4", INET_ADDRSTRLEN);

    printf("Function: %s, Line: %d\n", __func__, __LINE__);

    return 0;
}

int update_dhcp(const char *iface, int enable) {
    // Print debug info
    printf("Function: %s, Line: %d\n", __func__, __LINE__);
    printf("Input Parameters - Interface: %s, Enable: %d\n", iface, enable);

    // Mock database update logic

    return 0;
}


// SYSREPO DB HANDLE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysrepo.h>
#include <libyang/libyang.h>

void show_sysrepoctl_list(struct cli* cli)
{
    FILE *fp;
    char path[1035];

    // Open the command for reading
    fp = popen("sysrepoctl --list", "r");
    if (fp == NULL)
    {
        printf("Failed to run command\n");
        return;
    }

    // Read the output a line at a time and print it
    while (fgets(path, sizeof(path), fp) != NULL)
    {
        cli_out(cli, "%s", path);
    }

    // Close the file pointer
    pclose(fp);
}

void display_users_in_table(struct cli* cli, const char *xml_data) {

    char name[50], account_type[50], password[50], enabled[10];
    const char *start, *end;

    cli_out(cli, "Name       | Account Type | Password      | Enabled\n");
    cli_out(cli, "-----------|--------------|---------------|--------\n");

    if (xml_data == NULL)
    {
        cli_out(cli, "No data found. \n");
        xml_data = "<users xmlns=\"urn:o-ran:user-mgmt:1.0\">"
                           "<user>"
                           "<name>gigalane</name>"
                           "<account-type>PASSWORD</account-type>"
                           "<password>gigalane1234</password>"
                           "<enabled>true</enabled>"
                           "</user>"
                           "<user>"
                           "<name>admin</name>"
                           "<account-type>PASSWORD</account-type>"
                           "<password>admin1234</password>"
                           "<enabled>true</enabled>"
                           "</user>"
                           "</users>";
    }

    start = xml_data;
    while ((start = strstr(start, "<user>")) != NULL) {
        start = strstr(start, "<name>") + 6;
        end = strstr(start, "</name>");
        if (end == NULL) break;
        strncpy(name, start, end - start);
        name[end - start] = '\0';

        start = strstr(start, "<account-type>") + 14;
        end = strstr(start, "</account-type>");
        if (end == NULL) break;
        strncpy(account_type, start, end - start);
        account_type[end - start] = '\0';

        start = strstr(start, "<password>") + 10;
        end = strstr(start, "</password>");
        if (end == NULL) break;
        strncpy(password, start, end - start);
        password[end - start] = '\0';

        start = strstr(start, "<enabled>") + 9;
        end = strstr(start, "</enabled>");
        if (end == NULL) break;
        strncpy(enabled, start, end - start);
        enabled[end - start] = '\0';

        cli_out(cli, "%-10s | %-12s | %-13s | %-6s\n", name, account_type, password, enabled);

        start = end; // Move to the end of the current user entry
    }

    test_user();
}

int get_sysrepo_data(const char *xpath, char **result) {
    FILE *fp;
    char path[1035];
    size_t result_size = 0;

    // Command to fetch data from sysrepo
    char command[256];
    // snprintf(command, sizeof(command), "sudo sysrepocfg -d running -m %s -X", xpath);
    snprintf(command, sizeof(command), "sysrepocfg -d running -m %s -X", xpath);

    // Command string for debugging
    printf("Command: %s\n", command);

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command: %s\n", command);
        perror("popen");
        return 1;
    }

    // Read the output a line at a time and append to result
    *result = NULL;
    while (fgets(path, sizeof(path), fp) != NULL) {
        size_t len = strlen(path);
        char *new_result = realloc(*result, result_size + len + 1);
        if (new_result == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            free(*result);
            pclose(fp);
            return 1;
        }
        *result = new_result;
        strcpy(*result + result_size, path);
        result_size += len;
    }

    // Close the command
    pclose(fp);
    return 0;
}

int show_sysrepoctl_user_accout(struct cli *cli) {
    const char *xpath = "o-ran-usermgmt";
    char *result = NULL;

    // cli_out(cli, "Fetching data for XPath '%s'...\n", xpath);

    int rc = get_sysrepo_data(xpath, &result);
    if (rc != 0) {
        fprintf(stderr, "Failed to fetch data for XPath '%s'.\n", xpath);
    } else {
        display_users_in_table(cli, result);
        free(result); // Free the result buffer
    }

    return rc;
}

// get mplane interface info and vlan info
// Placeholder for TableRow structure
typedef struct {
    char key[MAX_LINE_LENGTH];
    char value[MAX_LINE_LENGTH];
} TableRow;

// Function to extract value from XML-like tags
void extract_value(const char *line, const char *tag, char *output) {
    char open_tag[MAX_LINE_LENGTH], close_tag[MAX_LINE_LENGTH];
    snprintf(open_tag, sizeof(open_tag), "<%s>", tag);
    snprintf(close_tag, sizeof(close_tag), "</%s>", tag);

    char *start = strstr(line, open_tag);
    if (start) {
        start += strlen(open_tag);
        char *end = strstr(start, close_tag);
        if (end) {
            strncpy(output, start, end - start);
            output[end - start] = '\0';
        }
    }
}

// Function to parse M-plane interface data
void oru_iface_mplane_parse_file(const char *file_path, TableRow *table, int *row_count) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    *row_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char value[MAX_LINE_LENGTH] = {0};

        // Parse the tags and fill the table rows
        if (strstr(line, "<searchable-access-vlans>")) {
            extract_value(line, "searchable-access-vlans", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Searchable VLAN");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<lowest-vlan-id>")) {
            extract_value(line, "lowest-vlan-id", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Lowest VLAN ID");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<highest-vlan-id>")) {
            extract_value(line, "highest-vlan-id", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Highest VLAN ID");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<scan-interval>")) {
            extract_value(line, "scan-interval", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Scan Interval");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }
    }

    fclose(file);
}

// Function to parse VLAN M-plane data
void oru_vlan_mplane_parse_file(const char *file_path, TableRow *table, int *row_count) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    *row_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char value[MAX_LINE_LENGTH] = {0};

        if (strstr(line, "<searchable-access-vlans>")) {
            extract_value(line, "searchable-access-vlans", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Searchable VLAN");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<lowest-vlan-id>")) {
            extract_value(line, "lowest-vlan-id", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Lowest VLAN ID");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<highest-vlan-id>")) {
            extract_value(line, "highest-vlan-id", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Highest VLAN ID");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }

        if (strstr(line, "<scan-interval>")) {
            extract_value(line, "scan-interval", value);
            snprintf(table[*row_count].key, sizeof(table[*row_count].key), "Scan Interval");
            strcpy(table[*row_count].value, value);
            (*row_count)++;
        }
    }

    fclose(file);
}

// Function to print table data
void print_table(struct cli *cli, TableRow *table, int row_count) {
    cli_out(cli, "%-25s | %-15s\n", "Parameter", "Value");
    printf("--------------------------------------------\n");
    for (int i = 0; i < row_count; i++) {
        cli_out(cli, "%-25s | %-15s\n", table[i].key, table[i].value);
    }
}

// Function to fetch M-plane data
int fetch_mplane_data(const char* module_name, const char* tmpfile) {
    char command[256];
    FILE* fp;
    char buffer[256];

    // Build the command string
    snprintf(command, sizeof(command),
             "sudo sysrepocfg --export=%s -d startup -m %s -e report-all",
             tmpfile, module_name);

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to run command: %s\n", command);
        perror("popen");
        return 1;  // Command execution failed
    }

    // Read the command output into the temporary file
    FILE* temp_fp = fopen(tmpfile, "w");
    if (!temp_fp) {
        perror("Error opening temporary file");
        pclose(fp);
        return 2;  // Failed to open temp file for writing
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        fputs(buffer, temp_fp);
    }

    fclose(temp_fp);
    pclose(fp);
    
    return 0;  // Success
}

// Function to show interface M-plane information
void show_iface_mplane_info(struct cli *cli)
{
    const char *tmpfile = "/tmp/mplane_int_info_tmp.XXXXXX"; // Temporary file
    int status = fetch_mplane_data("o-ran-mplane-int", tmpfile);
    if (status != 0) {
        fprintf(stderr, "Error: Failed to fetch M-plane interface data (status: %d)\n", status);
        return;
    }

    TableRow table[MAX_TABLE_ROWS];
    int row_count = 0;

    // Parse the M-plane data
    oru_iface_mplane_parse_file(tmpfile, table, &row_count);
    print_table(cli, table, row_count);

    // Delete the temporary file
    if (remove(tmpfile) != 0) {
        perror("Error: Failed to delete temporary file");
    }
}

// Function to show VLAN M-plane information
void show_vlan_mplane_info(struct cli *cli)
{
    const char *tmpfile = "/tmp/mplane_int_info_tmp.XXXXXX"; // Temporary file
    int status = fetch_mplane_data("o-ran-mplane-int", tmpfile);
    if (status != 0) {
        fprintf(stderr, "Error: Failed to fetch VLAN M-plane data (status: %d)\n", status);
        return;
    }

    TableRow table[MAX_TABLE_ROWS];
    int row_count = 0;

    // Parse the VLAN M-plane data
    oru_vlan_mplane_parse_file(tmpfile, table, &row_count);
    print_table(cli, table, row_count);

    // Delete the temporary file
    if (remove(tmpfile) != 0) {
        perror("Error: Failed to delete temporary file");
    }
}

// write a function to test mxml4

/// show user account using mxm4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mxml.h"

typedef struct {
    char name[50];
    char account_type[50];
    char password[50];
    int enabled;
} user_account_t;

void create_user_account_table(const char *xml_data) {
    mxml_node_t *tree, *node, *user;
    user_account_t users[10];
    int user_count = 0;

    // Parse the XML data
    tree = mxmlLoadString(NULL, xml_data, MXML_OPAQUE_CALLBACK);

    // Find the first user node
    node = mxmlFindElement(tree, tree, "user", NULL, NULL, MXML_DESCEND);

    // Iterate through all user nodes
    while (node != NULL && user_count < 10) {
        user = node;

        // Extract user information
        mxml_node_t *name_node = mxmlFindElement(user, user, "name", NULL, NULL, MXML_DESCEND);
        mxml_node_t *account_type_node = mxmlFindElement(user, user, "account-type", NULL, NULL, MXML_DESCEND);
        mxml_node_t *password_node = mxmlFindElement(user, user, "password", NULL, NULL, MXML_DESCEND);
        mxml_node_t *enabled_node = mxmlFindElement(user, user, "enabled", NULL, NULL, MXML_DESCEND);

        if (name_node && account_type_node && password_node && enabled_node) {
            strncpy(users[user_count].name, mxmlGetOpaque(name_node), sizeof(users[user_count].name) - 1);
            strncpy(users[user_count].account_type, mxmlGetOpaque(account_type_node), sizeof(users[user_count].account_type) - 1);
            strncpy(users[user_count].password, mxmlGetOpaque(password_node), sizeof(users[user_count].password) - 1);
            users[user_count].enabled = strcmp(mxmlGetOpaque(enabled_node), "true") == 0 ? 1 : 0;
            user_count++;
        }

        // Find the next user node
        node = mxmlFindElement(node, tree, "user", NULL, NULL, MXML_DESCEND);
    }

    // Print the user-account table
    printf("User-Account Table:\n");
    printf("Name\t\tAccount Type\tPassword\tEnabled\n");
    for (int i = 0; i < user_count; i++) {
        printf("%s\t%s\t%s\t%d\n", users[i].name, users[i].account_type, users[i].password, users[i].enabled);
    }

    // Free the XML tree
    mxmlDelete(tree);
}

void test_user() {
    const char *xml_data = "<users>"
                           "<user>"
                           "<name>gigalane</name>"
                           "<account-type>PASSWORD</account-type>"
                           "<password>gigalane1234</password>"
                           "<enabled>true</enabled>"
                           "</user>"
                           "<user>"
                           "<name>admin</name>"
                           "<account-type>PASSWORD</account-type>"
                           "<password>admin1234</password>"
                           "<enabled>true</enabled>"
                           "</user>"
                           "</users>";

    create_user_account_table(xml_data);
}