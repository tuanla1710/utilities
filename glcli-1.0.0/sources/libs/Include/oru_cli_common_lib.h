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
void show_ip_interface_brief(struct cli *cli);
int remove_ip_address(const char *iface, const char *ip, int mask);
int validate_and_convert_iface(char *iface);
void display_system_clock(struct cli *cli);
void display_system_cpu_load(struct cli* cli);
void show_memory_usage(struct cli* cli);

/* DB HANDLING */
int update_ip_address(const char *iface, const char *ip, int mask);
int check_ip_address(const char *iface, const char *ip, int mask);

int get_static_ip(const char *iface, char *ip, char *mask, char *gateway, char *dns1, char *dns2);
int update_dhcp(const char *iface, int enable);

/* sysrepoi db handle*/
void show_sysrepoctl_list(struct cli* cli);
int show_sysrepoctl_user_accout(struct cli *cli);

/* show mplane interface and vlan info*/
void show_vlan_mplane_info(struct cli *cli);
void show_iface_mplane_info(struct cli *cli);

void test_user(void);

#endif // ORU_CLI_COMMON_LIB_H