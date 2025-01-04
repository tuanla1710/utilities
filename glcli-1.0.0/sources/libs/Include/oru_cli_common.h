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

#include "oru_cli_common_define.h"

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

int32_t handle_system_show_sysrepo_info_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_system_show_sysrepo_info_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_system_show_sysrepo_info_resp(struct cli *cli, oru_general_msg_t* msg);

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

// Debug commands
int32_t handle_debug_cmd_test_cmd(struct cli *cli, oru_general_msg_t* msg);
int32_t handle_debug_cmd_test_req(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* resp);
void handle_debug_cmd_test_resp(struct cli *cli, oru_general_msg_t* msg);


// Common functions
void cli_out(struct cli *cli, const char *format, ...);
int32_t handle_command(struct cli *cli, oru_msg_type_e msg_type, uint16_t func_id, oru_general_msg_t* msg);
int32_t handle_request(struct cli *cli, oru_general_msg_t* req, oru_general_msg_t* rsp);
int32_t command_process(struct cli *cli);
uint16_t get_function_code(oru_msg_type_e msg_type, const char* command);

// oam request function
int handle_req_to_oam(struct cli* cli, oru_general_msg_t* req);

#endif // ORU_CLI_COMMON_H