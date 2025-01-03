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

bool show_interface_info(const char* ifname, char* output, size_t output_len);
int remove_ip_address(const char *iface, const char *ip, int mask);
int validate_and_convert_iface(char *iface);

/* DB HANDLING */
int update_ip_address(const char *iface, const char *ip, int mask);
int check_ip_address(const char *iface, const char *ip, int mask);

int get_static_ip(const char *iface, char *ip, char *mask, char *gateway, char *dns1, char *dns2);
int update_dhcp(const char *iface, int enable);

#endif // ORU_CLI_COMMON_LIB_H