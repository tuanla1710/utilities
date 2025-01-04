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

void show_ip_interface_brief(struct cli *cli) {
    FILE *fp;
    char path[1035];

    // Open the command for reading IP addresses
    fp = popen("ip -br addr", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    cli_out(cli, "Interface Summary:\n");
    cli_out(cli, "Name\t\tStatus\t\tIP Address\n");
    cli_out(cli, "-------------------------------------------------\n");

    // Read the output a line at a time and print it
    while (fgets(path, sizeof(path), fp) != NULL) {
        cli_out(cli, "%s", path);
    }

    // Close the file pointer
    pclose(fp);

    // Open the command for reading MAC addresses
    fp = popen("ip -br link", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return;
    }

    cli_out(cli, "\nMAC Address Summary:\n");
    cli_out(cli, "Name\t\tMAC Address\n");
    cli_out(cli, "-------------------------------------------------\n");

    // Read the output a line at a time and print it
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

int get_sysrepo_data(const char *xpath, char **result) {
    sr_conn_ctx_t *connection = NULL;
    sr_session_ctx_t *session = NULL;
    sr_data_t *data = NULL;
    int rc;

    if (!xpath || !result) {
        fprintf(stderr, "Invalid input to get_sysrepo_data\n");
        return -1; // Invalid input
    }

    *result = NULL; // Initialize the result to NULL

    // Connect to Sysrepo
    rc = sr_connect(SR_CONN_DEFAULT, &connection);
    if (rc != SR_ERR_OK) {
        fprintf(stderr, "Failed to connect to Sysrepo: %s\n", sr_strerror(rc));
        return rc;
    }

    // Start a session
    rc = sr_session_start(connection, SR_DS_RUNNING, &session);
    if (rc != SR_ERR_OK) {
        fprintf(stderr, "Failed to start session: %s\n", sr_strerror(rc));
        sr_disconnect(connection);
        return rc;
    }

    // Retrieve data
    rc = sr_get_data(session, xpath, 0, 0, 0, &data);
    if (rc != SR_ERR_OK || !data || !data->tree) {
        fprintf(stderr, "Failed to retrieve data for XPath '%s': %s\n", xpath, sr_strerror(rc));
        if (data) sr_release_data(data); // Clean up partially allocated resources
        sr_session_stop(session);
        sr_disconnect(connection);
        return rc;
    }

    // Serialize the data into XML format
    char *xml_str = NULL;
    rc = lyd_print_mem(&xml_str, data->tree, LYD_XML, LYD_PRINT_WITHSIBLINGS);
    if (rc != 0 || !xml_str) {
        fprintf(stderr, "Failed to serialize data\n");
        sr_release_data(data);
        sr_session_stop(session);
        sr_disconnect(connection);
        return rc;
    }

    // Assign the serialized data to the result
    *result = strdup(xml_str); // Copy the XML string to the result
    if (!*result) {
        fprintf(stderr, "Memory allocation failed\n");
        free(xml_str);
        sr_release_data(data);
        sr_session_stop(session);
        sr_disconnect(connection);
        return -1; // Memory allocation failure
    }

    // Clean up
    free(xml_str);
    sr_release_data(data);
    sr_session_stop(session);
    sr_disconnect(connection);

    return 0; // Success
}

void display_sysrepo_version(struct cli *cli)
{
    const char *xpath = "/sysrepo:sysrepo-version";
    char *result = NULL;

    cli_out(cli, "Fetching data for XPath '%s'...\n", xpath);

    int rc = get_sysrepo_data(xpath, &result);
    if (rc != 0) {
        fprintf(stderr, "Failed to fetch data for XPath '%s'.\n", xpath);
    } else {
        cli_out(cli, "Retrieved data:\n%s\n", result);
        printf("Retrieved data:\n%s\n", result);
        free(result); // Free the result buffer
    }
}


void print_sysrepo_data(struct cli *cli)
{
    const char *xpath = "/picocom-oru-state:*";
    char *result = NULL;

    cli_out(cli, "Fetching data for XPath '%s'...\n", xpath);

    int rc = get_sysrepo_data(xpath, &result);
    if (rc != 0) {
        fprintf(stderr, "Failed to fetch data for XPath '%s'.\n", xpath);
    } else {
        cli_out(cli, "Retrieved data:\n%s\n", result);
        printf("Retrieved data:\n%s\n", result);
        free(result); // Free the result buffer
    }
}

// #include <libxml/parser.h>
// #include <libxml/tree.h>
// #include <stdio.h>

// void display_slot_info(const char *xml_data) {
//     xmlDocPtr doc;
//     xmlNodePtr root, software_slot_info, active_slot_node, running_slot_node;
    
//     // Parse the XML data
//     doc = xmlParseMemory(xml_data, strlen(xml_data));
//     if (doc == NULL) {
//         fprintf(stderr, "Error: unable to parse the XML data.\n");
//         return;
//     }

//     // Get the root element
//     root = xmlDocGetRootElement(doc);
    
//     // Find the <software-slot-info> node
//     software_slot_info = NULL;
//     for (xmlNodePtr cur = root; cur != NULL; cur = cur->next) {
//         if (cur->type == XML_ELEMENT_NODE && strcmp((char*)cur->name, "software-slot-info") == 0) {
//             software_slot_info = cur;
//             break;
//         }
//     }

//     if (software_slot_info == NULL) {
//         fprintf(stderr, "Error: <software-slot-info> not found in XML.\n");
//         xmlFreeDoc(doc);
//         return;
//     }

//     // Find the <active_slot> and <running_slot> nodes
//     active_slot_node = NULL;
//     running_slot_node = NULL;
    
//     for (xmlNodePtr cur = software_slot_info->children; cur != NULL; cur = cur->next) {
//         if (cur->type == XML_ELEMENT_NODE) {
//             if (strcmp((char*)cur->name, "active_slot") == 0) {
//                 active_slot_node = cur;
//             }
//             if (strcmp((char*)cur->name, "running_slot") == 0) {
//                 running_slot_node = cur;
//             }
//         }
//     }

//     if (active_slot_node == NULL || running_slot_node == NULL) {
//         fprintf(stderr, "Error: <active_slot> or <running_slot> not found in XML.\n");
//         xmlFreeDoc(doc);
//         return;
//     }

//     // Extract the values of the active_slot and running_slot
//     char *active_slot = (char*)xmlNodeGetContent(active_slot_node);
//     char *running_slot = (char*)xmlNodeGetContent(running_slot_node);

//     // Display the information in a table format
//     printf("+-----------------+------------------+\n");
//     printf("| Field           | Value            |\n");
//     printf("+-----------------+------------------+\n");
//     printf("| Active Slot     | %-16s |\n", active_slot);
//     printf("| Running Slot    | %-16s |\n", running_slot);
//     printf("+-----------------+------------------+\n");

//     // Free allocated memory
//     xmlFree(active_slot);
//     xmlFree(running_slot);
//     xmlFreeDoc(doc);
// }