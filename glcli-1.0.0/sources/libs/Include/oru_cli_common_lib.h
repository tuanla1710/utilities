/*
 * GIGALANE PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with GIGALANE and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright GIGALANE.
 */

#ifndef ORU_CLI_COMMON_LIB_H
#define ORU_CLI_COMMON_LIB_H

#include <stdbool.h>

void show_interface_info(struct cli *cli, const char *ifname);
bool get_ifconfig_info(const char* ifname, char* output, size_t output_len);

#endif // ORU_CLI_COMMON_LIB_H