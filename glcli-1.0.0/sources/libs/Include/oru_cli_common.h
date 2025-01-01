/*
 * GIGALANE PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with GIGALANE and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright GIGALANE.
 */

#ifndef ORU_CLI_COMMON_H
#define ORU_CLI_COMMON_H

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

// vlan command msg type

// sync command msg type

// cplane command msg type

// User command msg type

// KPI command msg type


/*************************************************/
/*********** function define                   ***/
/*************************************************/
// System commands
int32_t handle_system_show_banner_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_banner_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_banner_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_inventory_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_inventory_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_inventory_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_system_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_system_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_system_info_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_write_memory_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_write_memory_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_write_memory_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_hostname_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_hostname_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_hostname_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_no_hostname_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_no_hostname_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_no_hostname_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_factory_defaults_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_factory_defaults_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_factory_defaults_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_save_config_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_save_config_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_save_config_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_restore_config_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_restore_config_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_restore_config_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_login_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_login_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_login_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_write_default_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_write_default_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_write_default_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_hostname_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_hostname_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_hostname_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_clock_set_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_clock_set_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_clock_set_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_clock_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_clock_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_clock_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_cpuload_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_cpuload_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_cpuload_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_memory_usage_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_memory_usage_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_memory_usage_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_cpuload_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_cpuload_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_cpuload_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_memory_usage_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_memory_usage_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_memory_usage_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_danger_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_danger_temperature_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_danger_temperature_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_high_high_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_high_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_high_high_temperature_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_high_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_high_temperature_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_low_temperature_threshold_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_low_temperature_threshold_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_low_temperature_threshold_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_temperature_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_temperature_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_temperature_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_system_software_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_system_software_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_system_software_info_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_system_show_system_image_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_system_image_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_system_image_info_resp(struct cli *cli, oru_general_msg_t* msg);

// Sync commands
int32_t handle_sync_show_ptp_status_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_show_ptp_status_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_show_ptp_status_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_sync_show_sync_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_show_sync_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_show_sync_info_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_sync_mode_ptp_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_mode_ptp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_mode_ptp_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_sync_mode_synce_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_mode_synce_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_mode_synce_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_sync_mode_freerun_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_mode_freerun_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_mode_freerun_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_sync_show_synce_status_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_sync_show_synce_status_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_sync_show_synce_status_resp(struct cli *cli, oru_general_msg_t* msg);

// Cplane commands
int32_t handle_cplane_show_oran_atten_status_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_show_oran_atten_status_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_show_oran_atten_status_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_cplane_show_pwr_status_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_show_pwr_status_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_show_pwr_status_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_cplane_show_port_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_show_port_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_show_port_info_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_cplane_power_amp_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_power_amp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_power_amp_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_cplane_tx_dpd_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_tx_dpd_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_tx_dpd_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_cplane_show_oran_prach_cplane_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_cplane_show_oran_prach_cplane_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_cplane_show_oran_prach_cplane_info_resp(struct cli *cli, oru_general_msg_t* msg);

// VLAN commands
int32_t handle_vlan_range_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_vlan_range_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_vlan_range_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_vlan_show_all_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_vlan_show_all_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_vlan_show_all_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_vlan_show_brief_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_vlan_show_brief_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_vlan_show_brief_resp(struct cli *cli, oru_general_msg_t* msg);

// IFACE commands
int32_t handle_iface_show_sfp_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_show_sfp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_show_sfp_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_ecpri_ul_mtu_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_ecpri_ul_mtu_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_ecpri_ul_mtu_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_ecpri_mac_destination_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_ecpri_mac_destination_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_ecpri_mac_destination_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_show_oran_ecpri_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_show_oran_ecpri_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_show_oran_ecpri_info_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_ecpri_vlan_control_enable_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_ecpri_vlan_control_enable_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_ecpri_vlan_control_enable_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_ecpri_vlan_control_disable_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_ecpri_vlan_control_disable_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_ecpri_vlan_control_disable_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_ecpri_vlan_id_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_ecpri_vlan_id_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_ecpri_vlan_id_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_oran_port_trunk_allowed_vlan_add_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_oran_port_trunk_allowed_vlan_add_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_oran_port_trunk_allowed_vlan_add_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_show_interface_brief_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_show_interface_brief_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_show_interface_brief_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_show_interface_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_show_interface_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_show_interface_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_ip_address_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_ip_address_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_ip_address_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_no_ip_address_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_no_ip_address_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_no_ip_address_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_dhcp_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_dhcp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_dhcp_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_no_dhcp_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_no_dhcp_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_no_dhcp_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_iface_show_ip_interface_brief_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_iface_show_ip_interface_brief_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_iface_show_ip_interface_brief_resp(struct cli *cli, oru_general_msg_t* msg);

// USER commands
int32_t handle_user_show_user_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_user_show_user_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_user_show_user_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_user_add_user_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_user_add_user_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_user_add_user_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_user_del_user_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_user_del_user_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_user_del_user_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_user_change_password_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_user_change_password_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_user_change_password_resp(struct cli *cli, oru_general_msg_t* msg);

// KPI commands
int32_t handle_kpi_show_statistics_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_kpi_show_statistics_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_kpi_show_statistics_resp(struct cli *cli, oru_general_msg_t* msg);

int32_t handle_kpi_clear_statistics_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_kpi_clear_statistics_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_kpi_clear_statistics_resp(struct cli *cli, oru_general_msg_t* msg);

// Common functions
void cli_out(struct cli *cli, const char *format, ...);
int32_t handle_command(struct cli *cli, oru_msg_type_e msg_type, uint16_t func_id, oru_general_msg_t* msg);
int32_t handle_request(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* rsp);
int32_t command_process(struct cli *cli);
uint16_t get_function_code(oru_msg_type_e msg_type, const char* command);

#endif // ORU_CLI_COMMON_H