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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
#include <utmp.h>

#include "oru_cli_common.h"
#include "oru_cli_common_lib.h"

const char *log_file = "/tmp/cli_out.txt";

void write_to_log(const char *log_file, const char *message) {
    FILE *file = fopen(log_file, "a");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    // Get the current time
    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Write the timestamp and message to the log file
    fprintf(file, "%s", message);
    // fprintf(file, "[%s] %s", time_str, message);

    fclose(file);
}

// cli_out function
void cli_out(struct cli *cli, const char *format, ...) {
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    write_to_log(log_file, buffer);
    cli = cli;
    // send(cli->sock, buffer, strlen(buffer), 0);
}

// lookup maximum function id value by each msg type
typedef struct{
    uint16_t msg_type;
    uint16_t max_func_id;
} max_func_id_t;

static max_func_id_t max_func_id_lookup[ORU_MSG_TYPE_NUM] = {
    {ORU_MSG_TYPE_SYSTEM, ORU_CMD_SYSTEM_NUM},
    {ORU_MSG_TYPE_SYNC, ORU_CMD_SYNC_NUM},
    {ORU_MSG_TYPE_CPLANE, ORU_CMD_CPLANE_NUM},
    {ORU_MSG_TYPE_VLAN, ORU_CMD_VLAN_NUM},
    {ORU_MSG_TYPE_IFACE, ORU_CMD_IFACE_NUM},
    {ORU_MSG_TYPE_USER, ORU_CMD_USER_NUM},
    {ORU_MSG_TYPE_KPI, ORU_CMD_KPI_NUM},
    {ORU_MSG_TYPE_DEBUG, ORU_CMD_DEBUG_NUM}
};

uint16_t get_max_func_id(oru_msg_type_e msg_type)
{
    return max_func_id_lookup[msg_type-1].max_func_id;
}

typedef int32_t (*ReqParser)(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp);
typedef int32_t (*CmdParser)(struct cli *cli, oru_general_msg_t* msg);
typedef void (*RespParser)(struct cli *cli, oru_general_msg_t* msg);

typedef struct {
    uint16_t func_id;
    ReqParser req_parser;
    CmdParser cmd_parser;
    RespParser resp_parser;
} cmd_list_t;

// Command list
static cmd_list_t cmd_list[ORU_MSG_TYPE_NUM][MAX_CMD_NUM] = {
    [ORU_MSG_TYPE_SYSTEM] = {
        {ORU_CMD_SYSTEM_SHOW_BANNER, handle_system_show_banner_req, handle_system_show_banner_cmd, handle_system_show_banner_resp},
        {ORU_CMD_SYSTEM_SHOW_INVENTORY, handle_system_show_inventory_req, handle_system_show_inventory_cmd, handle_system_show_inventory_resp},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_INFO, handle_system_show_system_info_req, handle_system_show_system_info_cmd, handle_system_show_system_info_resp},
        {ORU_CMD_SYSTEM_WRITE_MEMORY, handle_system_write_memory_req, handle_system_write_memory_cmd, handle_system_write_memory_resp},
        {ORU_CMD_SYSTEM_HOSTNAME, handle_system_hostname_req, handle_system_hostname_cmd, handle_system_hostname_resp},
        {ORU_CMD_SYSTEM_NO_HOSTNAME, handle_system_no_hostname_req, handle_system_no_hostname_cmd, handle_system_no_hostname_resp},
        {ORU_CMD_SYSTEM_FACTORY_DEFAULTS, handle_system_factory_defaults_req, handle_system_factory_defaults_cmd, handle_system_factory_defaults_resp},
        {ORU_CMD_SYSTEM_SAVE_CONFIG, handle_system_save_config_req, handle_system_save_config_cmd, handle_system_save_config_resp},
        {ORU_CMD_SYSTEM_RESTORE_CONFIG, handle_system_restore_config_req, handle_system_restore_config_cmd, handle_system_restore_config_resp},
        {ORU_CMD_SYSTEM_LOGIN, handle_system_login_req, handle_system_login_cmd, handle_system_login_resp},
        {ORU_CMD_SYSTEM_WRITE_DEFAULT, handle_system_write_default_req, handle_system_write_default_cmd, handle_system_write_default_resp},
        {ORU_CMD_SYSTEM_SHOW_HOSTNAME, handle_system_show_hostname_req, handle_system_show_hostname_cmd, handle_system_show_hostname_resp},
        {ORU_CMD_SYSTEM_CLOCK_SET, handle_system_clock_set_req, handle_system_clock_set_cmd, handle_system_clock_set_resp},
        {ORU_CMD_SYSTEM_SHOW_CLOCK, handle_system_show_clock_req, handle_system_show_clock_cmd, handle_system_show_clock_resp},
        {ORU_CMD_SYSTEM_CPULOAD_THRESHOLD, handle_system_cpuload_threshold_req, handle_system_cpuload_threshold_cmd, handle_system_cpuload_threshold_resp},
        {ORU_CMD_SYSTEM_MEMORY_USAGE_THRESHOLD, handle_system_memory_usage_threshold_req, handle_system_memory_usage_threshold_cmd, handle_system_memory_usage_threshold_resp},
        {ORU_CMD_SYSTEM_SHOW_CPULOAD, handle_system_show_cpuload_req, handle_system_show_cpuload_cmd, handle_system_show_cpuload_resp},
        {ORU_CMD_SYSTEM_SHOW_MEMORY_USAGE, handle_system_show_memory_usage_req, handle_system_show_memory_usage_cmd, handle_system_show_memory_usage_resp},
        {ORU_CMD_SYSTEM_DANGER_TEMPERATURE_THRESHOLD, handle_system_danger_temperature_threshold_req, handle_system_danger_temperature_threshold_cmd, handle_system_danger_temperature_threshold_resp},
        {ORU_CMD_SYSTEM_HIGH_HIGH_TEMPERATURE_THRESHOLD, handle_system_high_high_temperature_threshold_req, handle_system_high_high_temperature_threshold_cmd, handle_system_high_high_temperature_threshold_resp},
        {ORU_CMD_SYSTEM_HIGH_TEMPERATURE_THRESHOLD, handle_system_high_temperature_threshold_req, handle_system_high_temperature_threshold_cmd, handle_system_high_temperature_threshold_resp},
        {ORU_CMD_SYSTEM_LOW_TEMPERATURE_THRESHOLD, handle_system_low_temperature_threshold_req, handle_system_low_temperature_threshold_cmd, handle_system_low_temperature_threshold_resp},
        {ORU_CMD_SYSTEM_SHOW_TEMPERATURE, handle_system_show_temperature_req, handle_system_show_temperature_cmd, handle_system_show_temperature_resp},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_SOFTWARE_INFO, handle_system_show_system_software_info_req, handle_system_show_system_software_info_cmd, handle_system_show_system_software_info_resp},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_IMAGE_INFO, handle_system_show_system_image_info_req, handle_system_show_system_image_info_cmd, handle_system_show_system_image_info_resp},
        {ORU_CMD_SYSTEM_SHOW_SYSREPO_INFO, handle_system_show_sysrepo_info_req, handle_system_show_sysrepo_info_cmd, handle_system_show_sysrepo_info_resp},
    },
    [ORU_MSG_TYPE_SYNC] = {
        {ORU_CMD_SYNC_SHOW_PTP_STATUS, handle_sync_show_ptp_status_req, handle_sync_show_ptp_status_cmd, handle_sync_show_ptp_status_resp},
        {ORU_CMD_SYNC_SHOW_SYNC_INFO, handle_sync_show_sync_info_req, handle_sync_show_sync_info_cmd, handle_sync_show_sync_info_resp},
        {ORU_CMD_SYNC_MODE_PTP, handle_sync_mode_ptp_req, handle_sync_mode_ptp_cmd, handle_sync_mode_ptp_resp},
        {ORU_CMD_SYNC_MODE_SYNC_E, handle_sync_mode_synce_req, handle_sync_mode_synce_cmd, handle_sync_mode_synce_resp},
        {ORU_CMD_SYNC_MODE_FREERUN, handle_sync_mode_freerun_req, handle_sync_mode_freerun_cmd, handle_sync_mode_freerun_resp},
        {ORU_CMD_SYNC_SHOW_SYNCE_STATUS, handle_sync_show_synce_status_req, handle_sync_show_synce_status_cmd, handle_sync_show_synce_status_resp},
    },
    [ORU_MSG_TYPE_CPLANE] = {
        {ORU_CMD_CPLANE_SHOW_ORAN_ATTEN_STATUS, handle_cplane_show_oran_atten_status_req, handle_cplane_show_oran_atten_status_cmd, handle_cplane_show_oran_atten_status_resp},
        {ORU_CMD_CPLANE_SHOW_PWR_STATUS, handle_cplane_show_pwr_status_req, handle_cplane_show_pwr_status_cmd, handle_cplane_show_pwr_status_resp},
        {ORU_CMD_CPLANE_SHOW_PORT_INFO, handle_cplane_show_port_info_req, handle_cplane_show_port_info_cmd, handle_cplane_show_port_info_resp},
        {ORU_CMD_CPLANE_POWER_AMP, handle_cplane_power_amp_req, handle_cplane_power_amp_cmd, handle_cplane_power_amp_resp},
        {ORU_CMD_CPLANE_TX_DPD, handle_cplane_tx_dpd_req, handle_cplane_tx_dpd_cmd, handle_cplane_tx_dpd_resp},
        {ORU_CMD_CPLANE_SHOW_ORAN_PRACH_CPLANE_INFO, handle_cplane_show_oran_prach_cplane_info_req, handle_cplane_show_oran_prach_cplane_info_cmd, handle_cplane_show_oran_prach_cplane_info_resp},
    },
    [ORU_MSG_TYPE_VLAN] = {
        {ORU_CMD_VLAN_RANGE, handle_vlan_range_req, handle_vlan_range_cmd, handle_vlan_range_resp},
        {ORU_CMD_VLAN_SHOW_ALL, handle_vlan_show_all_req, handle_vlan_show_all_cmd, handle_vlan_show_all_resp},
        {ORU_CMD_VLAN_SHOW_BRIEF, handle_vlan_show_brief_req, handle_vlan_show_brief_cmd, handle_vlan_show_brief_resp},        
    },
    [ORU_MSG_TYPE_IFACE] = {
        {ORU_CMD_IFACE_SHOW_SFP, handle_iface_show_sfp_req, handle_iface_show_sfp_cmd, handle_iface_show_sfp_resp},
        {ORU_CMD_IFACE_ORAN_ECPRI_UL_MTU, handle_iface_oran_ecpri_ul_mtu_req, handle_iface_oran_ecpri_ul_mtu_cmd, handle_iface_oran_ecpri_ul_mtu_resp},
        {ORU_CMD_IFACE_ORAN_ECPRI_MAC_DESTINATION, handle_iface_oran_ecpri_mac_destination_req, handle_iface_oran_ecpri_mac_destination_cmd, handle_iface_oran_ecpri_mac_destination_resp},
        {ORU_CMD_IFACE_SHOW_ORAN_ECPRI_INFO, handle_iface_show_oran_ecpri_info_req, handle_iface_show_oran_ecpri_info_cmd, handle_iface_show_oran_ecpri_info_resp},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_ENABLE, handle_iface_oran_ecpri_vlan_control_enable_req, handle_iface_oran_ecpri_vlan_control_enable_cmd, handle_iface_oran_ecpri_vlan_control_enable_resp},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_DISABLE, handle_iface_oran_ecpri_vlan_control_disable_req, handle_iface_oran_ecpri_vlan_control_disable_cmd, handle_iface_oran_ecpri_vlan_control_disable_resp},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_ID, handle_iface_oran_ecpri_vlan_id_req, handle_iface_oran_ecpri_vlan_id_cmd, handle_iface_oran_ecpri_vlan_id_resp},
        {ORU_CMD_IFACE_ORAN_PORT_TRUNK_ALLOWED_VLAN_ADD, handle_iface_oran_port_trunk_allowed_vlan_add_req, handle_iface_oran_port_trunk_allowed_vlan_add_cmd, handle_iface_oran_port_trunk_allowed_vlan_add_resp},
        {ORU_CMD_IFACE_SHOW_INTERFACE_BRIEF, handle_iface_show_interface_brief_req, handle_iface_show_interface_brief_cmd, handle_iface_show_interface_brief_resp},
        {ORU_CMD_IFACE_SHOW_INTERFACE, handle_iface_show_interface_req, handle_iface_show_interface_cmd, handle_iface_show_interface_resp},
        {ORU_CMD_IFACE_IP_ADDRESS, handle_iface_ip_address_req, handle_iface_ip_address_cmd, handle_iface_ip_address_resp},
        {ORU_CMD_IFACE_NO_IP_ADDRESS, handle_iface_no_ip_address_req, handle_iface_no_ip_address_cmd, handle_iface_no_ip_address_resp},
        {ORU_CMD_IFACE_DHCP, handle_iface_dhcp_req, handle_iface_dhcp_cmd, handle_iface_dhcp_resp},
        {ORU_CMD_IFACE_NO_DHCP, handle_iface_no_dhcp_req, handle_iface_no_dhcp_cmd, handle_iface_no_dhcp_resp},
        {ORU_CMD_IFACE_SHOW_IP_INTERFACE_BRIEF, handle_iface_show_ip_interface_brief_req, handle_iface_show_ip_interface_brief_cmd, handle_iface_show_ip_interface_brief_resp},
    },
    [ORU_MSG_TYPE_USER] = {
        {ORU_CMD_USER_SHOW_USER, handle_user_show_user_req, handle_user_show_user_cmd, handle_user_show_user_resp},
        {ORU_CMD_USER_ADD_USER, handle_user_add_user_req, handle_user_add_user_cmd, handle_user_add_user_resp},
        {ORU_CMD_USER_DEL_USER, handle_user_del_user_req, handle_user_del_user_cmd, handle_user_del_user_resp},
        {ORU_CMD_USER_CHANGE_PASSWORD, handle_user_change_password_req, handle_user_change_password_cmd, handle_user_change_password_resp},
    },
    [ORU_MSG_TYPE_KPI] = {
        {ORU_CMD_KPI_SHOW_STATISTICS, handle_kpi_show_statistics_req, handle_kpi_show_statistics_cmd, handle_kpi_show_statistics_resp},
        {ORU_CMD_KPI_CLEAR_STATISTICS, handle_kpi_clear_statistics_req, handle_kpi_clear_statistics_cmd, handle_kpi_clear_statistics_resp},
    },
    [ORU_MSG_TYPE_DEBUG] = {
        {ORU_CMD_DEBUG_CMD_TEST, handle_debug_cmd_test_req, handle_debug_cmd_test_cmd, handle_debug_cmd_test_resp},
    }
};

typedef struct {
    char* cmd_name;
    uint32_t func_code;
} cmd_handler_t;

cmd_handler_t cmd_handler_list[ORU_MSG_TYPE_NUM][MAX_CMD_NUM+2] = {
    [ORU_MSG_TYPE_SYSTEM] = {
        {"show banner", ORU_CMD_SYSTEM_SHOW_BANNER},
        {"show inventory", ORU_CMD_SYSTEM_SHOW_INVENTORY},
        {"show system info", ORU_CMD_SYSTEM_SHOW_SYSTEM_INFO},
        {"write memory", ORU_CMD_SYSTEM_WRITE_MEMORY},
        {"hostname", ORU_CMD_SYSTEM_HOSTNAME},
        {"no hostname", ORU_CMD_SYSTEM_NO_HOSTNAME},
        {"factory-defaults", ORU_CMD_SYSTEM_FACTORY_DEFAULTS},
        {"save-config", ORU_CMD_SYSTEM_SAVE_CONFIG},
        {"restore-config", ORU_CMD_SYSTEM_RESTORE_CONFIG},
        {"login", ORU_CMD_SYSTEM_LOGIN},
        {"write default", ORU_CMD_SYSTEM_WRITE_DEFAULT},
        {"show hostname", ORU_CMD_SYSTEM_SHOW_HOSTNAME},
        {"clock set", ORU_CMD_SYSTEM_CLOCK_SET},
        {"show clock", ORU_CMD_SYSTEM_SHOW_CLOCK},
        {"cpuload threshold", ORU_CMD_SYSTEM_CPULOAD_THRESHOLD},
        {"memory-usage threshold", ORU_CMD_SYSTEM_MEMORY_USAGE_THRESHOLD},
        {"show cpuload", ORU_CMD_SYSTEM_SHOW_CPULOAD},
        {"show memory-usage", ORU_CMD_SYSTEM_SHOW_MEMORY_USAGE},
        {"danger termperature threshold", ORU_CMD_SYSTEM_DANGER_TEMPERATURE_THRESHOLD},
        {"high-high termperature threshold", ORU_CMD_SYSTEM_HIGH_HIGH_TEMPERATURE_THRESHOLD},
        {"high termperature threshold", ORU_CMD_SYSTEM_HIGH_TEMPERATURE_THRESHOLD},
        {"low termperature threshold", ORU_CMD_SYSTEM_LOW_TEMPERATURE_THRESHOLD},
        {"show temperature", ORU_CMD_SYSTEM_SHOW_TEMPERATURE},
        {"show system software info", ORU_CMD_SYSTEM_SHOW_SYSTEM_SOFTWARE_INFO},
        {"show system image info", ORU_CMD_SYSTEM_SHOW_SYSTEM_IMAGE_INFO},
        {"show sysrepo info", ORU_CMD_SYSTEM_SHOW_SYSREPO_INFO},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_SYNC] = {
        {"show ptp status", ORU_CMD_SYNC_SHOW_PTP_STATUS},
        {"show sync info", ORU_CMD_SYNC_SHOW_SYNC_INFO},
        {"sync mode ptp", ORU_CMD_SYNC_MODE_PTP},
        {"sync mode synce", ORU_CMD_SYNC_MODE_SYNC_E},
        {"sync mode freerun", ORU_CMD_SYNC_MODE_FREERUN},
        {"show synce status", ORU_CMD_SYNC_SHOW_SYNCE_STATUS},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_CPLANE] = {
        {"show oran atten status", ORU_CMD_CPLANE_SHOW_ORAN_ATTEN_STATUS},
        {"show pwr status", ORU_CMD_CPLANE_SHOW_PWR_STATUS},
        {"show port info", ORU_CMD_CPLANE_SHOW_PORT_INFO},
        {"power-amp", ORU_CMD_CPLANE_POWER_AMP},
        {"tx-dpd", ORU_CMD_CPLANE_TX_DPD},
        {"show oran prach cplane info", ORU_CMD_CPLANE_SHOW_ORAN_PRACH_CPLANE_INFO},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_VLAN] = {
        {"vlan range", ORU_CMD_VLAN_RANGE},
        {"show vlan all", ORU_CMD_VLAN_SHOW_ALL},
        {"show vlan brief", ORU_CMD_VLAN_SHOW_BRIEF},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_IFACE] = {
        {"show sfp", ORU_CMD_IFACE_SHOW_SFP},
        {"oran ecpri ul mtu", ORU_CMD_IFACE_ORAN_ECPRI_UL_MTU},
        {"oran ecpri mac destination", ORU_CMD_IFACE_ORAN_ECPRI_MAC_DESTINATION},
        {"show oran ecpri info", ORU_CMD_IFACE_SHOW_ORAN_ECPRI_INFO},
        {"oran ecpri vlan control enable", ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_ENABLE},
        {"oran ecpri vlan control disable", ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_DISABLE},
        {"oran ecpri vlan id", ORU_CMD_IFACE_ORAN_ECPRI_VLAN_ID},
        {"oran port trunk allowed vlan add", ORU_CMD_IFACE_ORAN_PORT_TRUNK_ALLOWED_VLAN_ADD},
        {"show interface brief", ORU_CMD_IFACE_SHOW_INTERFACE_BRIEF},
        {"show interface", ORU_CMD_IFACE_SHOW_INTERFACE},
        {"ip address", ORU_CMD_IFACE_IP_ADDRESS},
        {"no ip address", ORU_CMD_IFACE_NO_IP_ADDRESS},
        {"dhcp", ORU_CMD_IFACE_DHCP},
        {"no dhcp", ORU_CMD_IFACE_NO_DHCP},
        {"show ip interface brief", ORU_CMD_IFACE_SHOW_IP_INTERFACE_BRIEF},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_USER] = {
        {"show user", ORU_CMD_USER_SHOW_USER},
        {"add user", ORU_CMD_USER_ADD_USER},
        {"del user", ORU_CMD_USER_DEL_USER},
        {"change password", ORU_CMD_USER_CHANGE_PASSWORD},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_KPI] = {
        {"show oran kpi statistics", ORU_CMD_KPI_SHOW_STATISTICS},
        {"clear oran kpi statistics", ORU_CMD_KPI_CLEAR_STATISTICS},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    },
    [ORU_MSG_TYPE_DEBUG] = {
        {"cmd test", ORU_CMD_DEBUG_CMD_TEST},
        {"exit", MAX_CMD_NUM+ 1},  // Adding a unique code for exit
        {"help", MAX_CMD_NUM+ 2}   // Adding a unique code for help
    }

};

uint16_t get_function_code(oru_msg_type_e msg_type, const char* command) {
    for (size_t i = 0; i < MAX_CMD_NUM; i++) {
        if (cmd_handler_list[msg_type][i].cmd_name != NULL &&
            strncmp(command, cmd_handler_list[msg_type][i].cmd_name, strlen(cmd_handler_list[msg_type][i].cmd_name)) == 0) {
            return cmd_handler_list[msg_type][i].func_code;
        }
    }
    return MAX_CMD_NUM + 3;  // Return a unique code for unknown commands
}
int32_t handle_command(struct cli* cli, oru_msg_type_e msg_type, uint16_t func_id, oru_general_msg_t* msg) {
    for (int i = 0; i < MAX_CMD_NUM; i++) {
        // printf("Checking func_id = %d against cmd_list[%d][%d].func_id = %d\n", func_id, msg_type, i, cmd_list[msg_type][i].func_id);
        if (func_id == cmd_list[msg_type][i].func_id) {
            // printf("Found matching func_id = %d, calling cmd_parser\n", func_id);
            return cmd_list[msg_type][i].cmd_parser(cli, msg);
        }
    }

    cli_out(cli, "Error: Command not found for func_id = %d\n", func_id);
    return 1;  // Return error if command not found
}

int32_t  handle_response(struct cli *cli, uint16_t func_id, oru_general_msg_t* msg) {
    oru_msg_type_e msg_type = msg->header.msg_type;
    for (int i = 0; i < MAX_CMD_NUM; i++) {
        if (func_id == cmd_list[msg_type][i].func_id) {
            cmd_list[msg_type][i].resp_parser(cli, msg);
            return 0;
        }
    }
    cli_out(cli, "Error: not found func_id = %d, mode_id = %d, cmd=%s\n", func_id, msg_type, cli->cmd);

    return -1;
}

// Server handle
int32_t handle_request(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    oru_msg_type_e msg_type = req->header.msg_type;
    uint16_t func_id = req->header.func_id;

    // check if msg_type is valid
    if (msg_type >= ORU_MSG_TYPE_NUM || !msg_type)
    {
        cli_out(cli, "Error: Invalid msg_type = %d\n", msg_type);
        return -1;
    }

    // check if func_id is valid
    if (func_id >= get_max_func_id(msg_type) || req->header.func_id == 0)
    {
        cli_out(cli, "Msg_type = %d\n, max_func_id = %d", msg_type, get_max_func_id(msg_type));
        cli_out(cli, "Error: Invalid func_id = %d\n", req->header.func_id);
        return -1;
    }

    for (int i = 0; i < MAX_CMD_NUM; i++) {
        // printf("Checking func_id = %d against cmd_list[%d][%d].func_id = %d\n", func_id, msg_type, i, cmd_list[msg_type][i].func_id);
        if (func_id == cmd_list[msg_type][i].func_id) {
            // printf("Found matching func_id = %d, calling req_parser\n", func_id);
            return cmd_list[msg_type][i].req_parser(cli, req, resp);
        }
    }
    cli_out(cli, "Error: Request handler not found for func_id = %d\n", func_id);

    return -1;
}

// Command handlers
int32_t handle_system_show_banner_cmd(struct cli *cli, oru_general_msg_t* msg) {
    cli = cli;
    msg = msg;

    return 0;
}

int32_t handle_system_show_banner_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_banner_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_inventory_cmd(struct cli *cli, oru_general_msg_t* msg) {
    cli = cli;
    msg = msg;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_inventory_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_inventory_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_system_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_system_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_system_info_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_write_memory_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_write_memory_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_write_memory_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_hostname_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_hostname_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_hostname_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
}

int32_t handle_system_no_hostname_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_no_hostname_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_no_hostname_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_factory_defaults_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_factory_defaults_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_factory_defaults_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_save_config_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_save_config_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_save_config_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_restore_config_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_restore_config_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_restore_config_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_login_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_login_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_login_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_write_default_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_write_default_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_write_default_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_hostname_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_hostname_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_show_hostname_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_clock_set_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_clock_set_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_clock_set_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_clock_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_clock_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_show_clock_resp(struct cli* cli, oru_general_msg_t* msg) {

    /* show clock */
    display_system_clock(cli);

    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_cpuload_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_cpuload_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_cpuload_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_memory_usage_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_memory_usage_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_memory_usage_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_cpuload_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_cpuload_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_show_cpuload_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;

    /* show cpuload */
    display_system_cpu_load(cli);

    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_memory_usage_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_memory_usage_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_show_memory_usage_resp(struct cli* cli, oru_general_msg_t* msg) {

    /* show memory-usage */
    show_memory_usage(cli);

    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_temperature_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_temperature_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_temperature_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_danger_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_danger_temperature_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_danger_temperature_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_high_high_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_high_high_temperature_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_high_high_temperature_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_high_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_high_temperature_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_high_temperature_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {

    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_low_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_low_temperature_threshold_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_low_temperature_threshold_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_system_software_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_system_software_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_system_software_info_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_system_image_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_system_image_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_system_show_system_image_info_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_system_show_sysrepo_info_cmd(struct cli *cli, oru_general_msg_t* msg) {
    cli = cli;
    msg = msg;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_system_show_sysrepo_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_system_show_sysrepo_info_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;

    show_sysrepoctl_list(cli);
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
}

// Sync commands
int32_t handle_sync_show_ptp_status_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_show_ptp_status_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_show_ptp_status_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_sync_show_sync_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_show_sync_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_show_sync_info_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_sync_mode_ptp_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_mode_ptp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_mode_ptp_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_sync_mode_synce_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_mode_synce_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_mode_synce_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_sync_mode_freerun_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_mode_freerun_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_mode_freerun_resp(struct cli* cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_sync_show_synce_status_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_sync_show_synce_status_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_sync_show_synce_status_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

// Cplane commands
int32_t handle_cplane_show_oran_atten_status_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_show_oran_atten_status_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_cplane_show_oran_atten_status_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_cplane_show_pwr_status_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_show_pwr_status_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_cplane_show_pwr_status_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_cplane_show_port_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_show_port_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_cplane_show_port_info_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_cplane_power_amp_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_power_amp_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_cplane_power_amp_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_cplane_tx_dpd_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_tx_dpd_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_cplane_tx_dpd_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_cplane_show_oran_prach_cplane_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_cplane_show_oran_prach_cplane_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_cplane_show_oran_prach_cplane_info_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

// VLAN commands
int32_t handle_vlan_range_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_vlan_range_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_vlan_range_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_vlan_show_all_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_vlan_show_all_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_vlan_show_all_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;

    show_vlan_mplane_info(cli);
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_vlan_show_brief_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_vlan_show_brief_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_vlan_show_brief_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}


// IF commands
int32_t handle_iface_show_sfp_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_show_sfp_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_show_sfp_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_ecpri_ul_mtu_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_ecpri_ul_mtu_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_ecpri_ul_mtu_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_ecpri_mac_destination_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_ecpri_mac_destination_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_ecpri_mac_destination_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_show_oran_ecpri_info_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_show_oran_ecpri_info_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_show_oran_ecpri_info_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_ecpri_vlan_control_enable_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_ecpri_vlan_control_enable_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_ecpri_vlan_control_enable_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_ecpri_vlan_control_disable_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_ecpri_vlan_control_disable_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_ecpri_vlan_control_disable_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_ecpri_vlan_id_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_ecpri_vlan_id_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_ecpri_vlan_id_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_oran_port_trunk_allowed_vlan_add_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_oran_port_trunk_allowed_vlan_add_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_iface_oran_port_trunk_allowed_vlan_add_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_show_interface_brief_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_show_interface_brief_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_iface_show_interface_brief_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;

    show_ip_interface_brief(cli);

    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_show_interface_cmd(struct cli *cli, oru_general_msg_t* msg) {
    char iface[25] = {0};

    // get interface name from "show interface IFNAME"
    if (sscanf(cli->cmd, "show interface %24s", iface) == 1) {
        printf("Interface name: %s\n", iface);
    } else {
        cli_out(cli, "Error - Invalid command: %s\n", cli->cmd);
        return -1;
    }

    // Validate and convert the interface
    if (validate_and_convert_iface(iface) != 0)
    {
        cli_out(cli, "Error - Invalid interface: %s\n", iface);
        return -1; // Error
    }

    msg->body.param_num = sizeof(oru_iface_show_interface_req_t) >> 2; // num of int32_t
    // Update the interface name in the message
    oru_iface_show_interface_req_t *req = (oru_iface_show_interface_req_t *)&msg->body.param_value[0];

    // Update the interface name in the message
    strncpy(req->ifacename, iface, sizeof(iface)-1);
    req->ifacename[sizeof(req->ifacename) - 1] = '\0'; // Ensure null-termination

    cli = cli;
    printf("%s: %s\n", __func__, cli->cmd);

    return 0;
}

int32_t handle_iface_show_interface_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    
    int32_t errorStatus = 0;
    
    oru_iface_show_interface_req_t *creq = (oru_iface_show_interface_req_t *)&req->body.param_value[0];

    resp->body.param_num = sizeof(oru_iface_show_interface_req_t) >> 2; // num of int32_t
    oru_iface_show_interface_req_t *cresp = (oru_iface_show_interface_req_t *)&resp->body.param_value[0];

    // Print the original interface name in req
    printf("Original req->ifacename: %s\n", creq->ifacename);

    // Copy the interface name from req to resp
    strncpy(cresp->ifacename, creq->ifacename, sizeof(creq->ifacename) - 1);
    cresp->ifacename[sizeof(cresp->ifacename) - 1] = '\0'; // Ensure null-termination

    // Print the copied interface name in resp
    printf("Copied resp->ifacename: %s\n", cresp->ifacename);

    // Update error status
    cresp->error_status = errorStatus;

    // Update the message type and function id
    resp->header.func_id = req->header.func_id;
    resp->header.msg_type = req->header.msg_type;
    resp->header.msg_size = sizeof(oru_iface_show_interface_req_t) + sizeof(resp->body.param_num); 

    cli = cli;
    printf("%s: %s\n", __func__, cli->cmd);

    return 0;
}

void handle_iface_show_interface_resp(struct cli *cli, oru_general_msg_t *msg)
{

    cli = cli;
    msg = msg;

    char output[MAX_CMD_OUTPUT_LEN] = {0};
    oru_iface_show_interface_req_t *resp = (oru_iface_show_interface_req_t *)&msg->body.param_value[0];
    printf("Interface name: %s\n", resp->ifacename);

    if (strcmp(resp->ifacename, "mplane") == 0)
    {
        show_iface_mplane_info(cli);
    }
    else
    {
        show_interface_info(resp->ifacename, output, sizeof(output));
    }

    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_ip_address_cmd(struct cli *cli, oru_general_msg_t* msg) {
    // Validate input
    if (cli == NULL || msg == NULL) {
        return -1;
    }

    // Print the command for debugging
    printf("%s: %s\n", __func__, cli->cmd);

    // Expected command format: "ip address <A.B.C.D>/<M> interface <IFNAME>"
    char ip[20];
    char iface[20];
    int mask;

    // Parse the command
    if (sscanf(cli->cmd, "ip address %19[^/]/%d interface %19s", ip, &mask, iface) == 3) {
        // Successfully parsed the command
        printf("IP Address: %s\n", ip);
        printf("Mask: %d\n", mask);
        printf("Interface: %s\n", iface);

        // Validate and convert the interface
        if (validate_and_convert_iface(iface) != 0) {
            cli_out(cli, "Error - Invalid interface: %s\n", iface);
            return -1; // Error
        }

    } else {
        // Failed to parse the command
        cli_out(cli, "Error - Invalid command: %s\n", cli->cmd);
        return -1; // Error
    }

    // Update the message
    oru_iface_ip_address_req_t *req = (oru_iface_ip_address_req_t *)&msg->body.param_value[0];
    msg->body.param_num = sizeof(oru_iface_ip_address_req_t) >> 2; // num of int32_t

    strncpy(req->ip, ip, sizeof(ip) - 1);
    req->ip[sizeof(req->ip) - 1] = '\0'; // Ensure null-termination
    req->mask = mask;
    strncpy(req->iface, iface, sizeof(iface) - 1);
    req->iface[sizeof(req->iface) - 1] = '\0'; // Ensure null-termination

    return 0; // Success
}

int32_t handle_iface_ip_address_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    int32_t errorStatus = 0;

    oru_iface_ip_address_req_t *creq = (oru_iface_ip_address_req_t *)&req->body.param_value[0];

    resp->body.param_num = sizeof(oru_iface_ip_address_resp_t) >> 2; // num of int32_t 
    oru_iface_ip_address_resp_t *cresp = (oru_iface_ip_address_resp_t *)&resp->body.param_value[0];

    if (check_ip_address(creq->iface, creq->ip, creq->mask) == 0) {
        // Success
        printf("IP address already exists in the database\n");
        errorStatus = 0;
    } else {
        // Failed
        printf("IP address does not exist in the database\n");
        errorStatus = -1;
    }

    // set IP to interface linux
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "ip addr add %s/%d dev %s", creq->ip, creq->mask, creq->iface);

    // Execute the command
    if (system(cmd) == 0) {
        // Success
        printf("Command executed successfully: %s\n", cmd);
    } else {
        // Failed
        printf("Failed to execute command: %s\n", cmd);
        errorStatus = -1;
    }

    // Update IP address to database
    if (errorStatus == 0) {
        // Update the IP address to the database
        if (update_ip_address(creq->iface, creq->ip, creq->mask) == 0) {
            // Success
            printf("IP address added to database\n");
        } else {
            // Failed
            printf("Failed to add IP address to database\n");
            errorStatus = -1;
        }
    }

    // Restart the networking service
    if (system("systemctl restart networking") != 0) {
        printf("Failed to restart networking service\n");
        errorStatus = -1;
    } else {
        printf("Networking service restarted\n");
    }

    // Update error status
    cresp->error_status = errorStatus;

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_iface_ip_address_resp(struct cli *cli, oru_general_msg_t* msg) {

    oru_iface_ip_address_resp_t *resp = (oru_iface_ip_address_resp_t *)&msg->body.param_value[0];
    if (resp->error_status == 0) {
        cli_out(cli, "IP address added successfully\n");
    } else {
        cli_out(cli, "Failed to add IP address\n");
    }

    // msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_no_ip_address_cmd(struct cli *cli, oru_general_msg_t* msg) {

    // Validate input
    if (cli == NULL || msg == NULL) {
        return -1;
    }

    // printf the command for debugging
    printf("%s: %s\n", __func__, cli->cmd);

    // Expected command format: "no ip address <A.B.C.D>/<M> interface <IFNAME>"
    char ip[20];
    char iface[20];
    int mask;

    // Parse the command
    if (sscanf(cli->cmd, "no ip address %19[^/]/%d interface %19s", ip, &mask, iface) == 3) {
        // Successfully parsed the command
        printf("IP Address: %s\n", ip);
        printf("Mask: %d\n", mask);
        printf("Interface: %s\n", iface);

        // Validate and convert the interface
        if (validate_and_convert_iface(iface) != 0) {
            cli_out(cli, "Error - Invalid interface: %s\n", iface);
            return -1; // Error
        }

    } else {
        // Failed to parse the command
        cli_out(cli, "Error - Invalid command: %s\n", cli->cmd);
        return -1; // Error
    }

    // Update the message
    oru_iface_no_ip_address_req_t *req = (oru_iface_no_ip_address_req_t *)&msg->body.param_value[0];
    msg->body.param_num = sizeof(oru_iface_no_ip_address_req_t) >> 2; // num of int32_t

    strncpy(req->ip, ip, sizeof(ip) - 1);
    req->ip[sizeof(req->ip) - 1] = '\0'; // Ensure null-termination
    req->mask = mask;
    strncpy(req->iface, iface, sizeof(iface) - 1);
    req->iface[sizeof(req->iface) - 1] = '\0'; // Ensure null-termination

    printf("%s: %s\n", __func__, cli->cmd);

    return 0; // Success
}

int32_t handle_iface_no_ip_address_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    int32_t errorStatus = 0;

    oru_iface_no_ip_address_req_t *creq = (oru_iface_no_ip_address_req_t *)&req->body.param_value[0];

    resp->body.param_num = sizeof(oru_iface_no_ip_address_resp_t) >> 2; // num of int32_t
    oru_iface_no_ip_address_resp_t *cresp = (oru_iface_no_ip_address_resp_t *)&resp->body.param_value[0];

    // Construct the system command to remove IP address
    char command[100] = {0};
    snprintf(command, sizeof(command), "ip addr del %s/%d dev %s", creq->ip, creq->mask, creq->iface);

    // Execute the system command
    if (system(command) == 0) {
        // Success
        printf("IP address removed successfully\n");
    } else {
        // Failed
        printf("Failed to remove IP address\n");
        errorStatus = -1;
    }

    // Restart the networking service
    if (system("systemctl restart networking") != 0) {
        printf("Failed to restart networking service\n");
        errorStatus = -1;
    } else {
        printf("Networking service restarted\n");
    }

    // Update error status
    cresp->error_status = errorStatus;

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_iface_no_ip_address_resp(struct cli *cli, oru_general_msg_t* msg) {

    oru_iface_no_ip_address_resp_t *resp = (oru_iface_no_ip_address_resp_t *)&msg->body.param_value[0];
    if (resp->error_status == 0) {
        cli_out(cli, "IP address removed successfully\n");
    } else {
        cli_out(cli, "Failed to remove IP address\n");
    }

    // msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_dhcp_cmd(struct cli *cli, oru_general_msg_t* msg) {

    // Validate input
    if (cli == NULL || msg == NULL) {
        return -1;
    }

    // printf the command for debugging
    printf("%s: %s\n", __func__, cli->cmd);

    // Expected command format: "dhcp interface <IFNAME>"
    char iface[20];

    // Parse the command
    if (sscanf(cli->cmd, "dhcp interface %19s", iface) == 1) {
        // Successfully parsed the command
        printf("Interface: %s\n", iface);

        // Validate and convert the interface
        if (validate_and_convert_iface(iface) != 0) {
            cli_out(cli, "Error - Invalid interface: %s\n", iface);
            return -1; // Error
        }         

    } else {
        // Failed to parse the command
        printf("Invalid command format\n");
        return -1; // Error
    }

    // Update the message
    oru_iface_dhcp_req_t *req = (oru_iface_dhcp_req_t *)&msg->body.param_value[0];
    msg->body.param_num = sizeof(oru_iface_dhcp_req_t) >> 2;

    strncpy(req->iface, iface, sizeof(iface) - 1);
    req->iface[sizeof(req->iface) - 1] = '\0'; // Ensure null-termination

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_dhcp_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    int32_t errorStatus = 0;

    oru_iface_dhcp_req_t *creq = (oru_iface_dhcp_req_t *)&req->body.param_value[0];
    resp->body.param_num = sizeof(oru_iface_dhcp_resp_t) >> 2; // num of int32_t
    oru_iface_dhcp_resp_t *cresp = (oru_iface_dhcp_resp_t *)&resp->body.param_value[0];

    // Construct the system command to enable DHCP
    char command[256];
    snprintf(command, sizeof(command), "dhclient %s", creq->iface);

    // Execute the system command
    if (system(command) != 0) {
        printf("Failed to enable DHCP on interface %s\n", creq->iface);
        errorStatus = -1;
    } else {
        printf("DHCP enabled on interface %s\n", creq->iface);

        // Restart the networking service
        if (system("systemctl restart networking") != 0) {
            printf("Failed to restart networking service\n");
            errorStatus = -1;
        } else {
            printf("Networking service restarted\n");
        }

        // Update the database
        if (update_dhcp(creq->iface, 1) != 0) {
            printf("Failed to enable DHCP in the database\n");
            errorStatus = -1;
        } else {
            printf("DHCP enabled in the database\n");
        }
    }

    // Update error status
    cresp->error_status = errorStatus;

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);

    return errorStatus;
}

void handle_iface_dhcp_resp(struct cli *cli, oru_general_msg_t* msg) {

    oru_iface_dhcp_resp_t *resp = (oru_iface_dhcp_resp_t *)&msg->body.param_value[0];

    if (resp->error_status == 0) {
        cli_out(cli, "DHCP enabled successfully\n");
    } else {
        cli_out(cli, "Failed to enable DHCP\n");
    }

    msg = msg;
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_no_dhcp_cmd(struct cli *cli, oru_general_msg_t* msg) {
    // Validate input
    if (cli == NULL || msg == NULL) {
        return -1;
    }

    // Print the command for debugging
    printf("%s: %s\n", __func__, cli->cmd);

    // Expected command format: "no dhcp interface <IFNAME>"
    char iface[20];

    // Parse the command
    if (sscanf(cli->cmd, "no dhcp interface %19s", iface) != 1) {
        // Failed to parse the command
        printf("Invalid command format\n");
        return -1; // Error
    }

    // Successfully parsed the command
    printf("Interface: %s\n", iface);

    // Validate and convert the interface
    if (validate_and_convert_iface(iface) != 0) {
        cli_out(cli, "Error - Invalid interface: %s\n", iface);
        return -1; // Error
    }

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_no_dhcp_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {
    int32_t errorStatus = 0;

    oru_iface_no_dhcp_req_t *creq = (oru_iface_no_dhcp_req_t *)&req->body.param_value[0];
    resp->body.param_num = sizeof(oru_iface_no_dhcp_resp_t) >> 2; // num of int32_t
    oru_iface_no_dhcp_resp_t *cresp = (oru_iface_no_dhcp_resp_t *)&resp->body.param_value[0];

    // Construct and execute the system command to disable DHCP
    char command[512];
    snprintf(command, sizeof(command), "dhclient -r %s", creq->iface);
    if (system(command) != 0) {
        printf("Failed to disable DHCP on interface %s\n", creq->iface);
        errorStatus = -1;
    } else {
        printf("DHCP disabled on interface %s\n", creq->iface);
    }

    // Get static IP from database
    char ip[20], mask[20], gateway[20], dns1[20], dns2[20];
    if (get_static_ip(creq->iface, ip, mask, gateway, dns1, dns2) != 0) {
        printf("Failed to retrieve static IP address from database\n");
        errorStatus = -1;
    } else {
        printf("Static IP address retrieved from database\n");
    }

    // Construct and execute the command to set static IP
    snprintf(command, sizeof(command), "sudo sed -i '/iface %s inet/c\\iface %s inet static\\naddress %s\\nnetmask %s\\ngateway %s\\ndns-nameservers %s %s' /etc/network/interfaces",
             creq->iface, creq->iface, ip, mask, gateway, dns1, dns2);
    if (system(command) != 0) {
        printf("Failed to execute command: %s\n", command);
        errorStatus = -1;
    } else {
        printf("Command executed successfully: %s\n", command);
    }

    // Restart the networking service
    if (system("systemctl restart networking") != 0) {
        printf("Failed to restart networking service\n");
        errorStatus = -1;
    } else {
        printf("Networking service restarted\n");
    }

    // Update the database if no errors occurred
    if (errorStatus == 0 && update_dhcp(creq->iface, 0) != 0) {
        printf("Failed to disable DHCP in the database\n");
        errorStatus = -1;
    } else if (errorStatus == 0) {
        printf("DHCP disabled in the database\n");
    }

    // Update error status
    cresp->error_status = errorStatus;

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);

    return errorStatus;
}

void handle_iface_no_dhcp_resp(struct cli *cli, oru_general_msg_t* msg) {

    oru_iface_no_dhcp_resp_t *resp = (oru_iface_no_dhcp_resp_t *)&msg->body.param_value[0];

    if (resp->error_status == 0) {
        cli_out(cli, "DHCP enabled successfully\n");
    } else {
        cli_out(cli, "Failed to enable DHCP\n");
    }

    msg = msg;
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_iface_show_ip_interface_brief_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_iface_show_ip_interface_brief_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_iface_show_ip_interface_brief_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    show_ip_interface_brief(cli);
    printf("%s: %s\n", __func__, cli->cmd);
}

// User commands
int32_t handle_user_show_user_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_user_show_user_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_user_show_user_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);

    show_sysrepoctl_user_accout(cli);
    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_user_add_user_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_user_add_user_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_user_add_user_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);

    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_user_del_user_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_user_del_user_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_user_del_user_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);

    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_user_change_password_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_user_change_password_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;
    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

void handle_user_change_password_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    printf("%s: %s\n", __func__, cli->cmd);
}

// KPI commands
int32_t handle_kpi_show_statistics_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_kpi_show_statistics_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_kpi_show_statistics_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);

    printf("%s: %s\n", __func__, cli->cmd);
}

int32_t handle_kpi_clear_statistics_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_kpi_clear_statistics_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_kpi_clear_statistics_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);

    printf("%s: %s\n", __func__, cli->cmd);
}

// Command debug
int32_t handle_debug_cmd_test_cmd(struct cli *cli, oru_general_msg_t* msg) {

    cli = cli;
    msg = msg;

    printf("%s: %s\n", __func__, cli->cmd);
    return 0;
}

int32_t handle_debug_cmd_test_req(struct cli* cli, oru_general_msg_t* req, oru_general_msg_t* resp) {

    cli = cli;
    resp->header.msg_type = req->header.msg_type;
    resp->header.func_id = req->header.func_id;

    printf("%s: %s\n", __func__, cli->cmd);
    return handle_req_to_oam(cli, req);
}

void handle_debug_cmd_test_resp(struct cli *cli, oru_general_msg_t* msg) {
    msg = msg;
    cli = cli;
    // cli_out(cli, "Error - Not supported: %s, id = %d\n", cli->cmd, msg->header.func_id);
    // display_sysrepo_version(cli);

    // display_sysrepoctl_list(cli);
    printf("%s: %s\n", __func__, cli->cmd);

    // show_sysrepoctl_list(cli);

    printf("%s: %s\n", __func__, cli->cmd);
}

void clear_log(const char *log_file)
{
    FILE *file = fopen(log_file, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    // Clear all content in the file
    file = fopen(log_file, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    fclose(file);
}

// Command processing
int32_t command_process(struct cli* cli) {

    int32_t resp_buffer[BUFFER_SIZE] = {0};
    int32_t req_buffer[BUFFER_SIZE] = {0};

    oru_general_msg_t* req = (oru_general_msg_t*)&req_buffer[0];
    oru_general_msg_t* resp = (oru_general_msg_t*)&resp_buffer[0];

    clear_log(log_file);

    uint16_t func_id = get_function_code(cli->mode, cli->cmd);
    if (func_id >= MAX_CMD_NUM + 1) {
        cli_out(cli, "Invalid command: %s\n", cli->cmd);
        return -1;
    }

    // command request handler
    if (handle_command(cli, cli->mode, func_id, req) != 0) {
        cli_out(cli, "Error handling command: %s\n", cli->cmd);
        return -1;
    }

    // Set the message header
    req->header.msg_type = cli->mode;
    req->header.func_id = func_id;
    req->header.msg_size = MSG_BODY_SIZE(req->body.param_num) + sizeof(oru_msg_header_t);

    // command response handler
    if (handle_request(cli, req, resp) != 0) {
        cli_out(cli, "Error handling request command: %s\n", cli->cmd);
        return -1;
    }

    // command response handler
    handle_response(cli, func_id, resp);

    return 0;
}