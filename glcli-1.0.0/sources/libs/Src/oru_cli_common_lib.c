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
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "oru_cli_common.h"

bool get_ifconfig_info(const char* ifname, char* output, size_t output_len) {
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

void show_interface_info(struct cli *cli, const char *ifname) {
    cli_out(cli, "ifconfig %s\n", ifname);
}