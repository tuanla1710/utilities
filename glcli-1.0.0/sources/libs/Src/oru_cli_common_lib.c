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

