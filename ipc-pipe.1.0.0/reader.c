#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#include "oru_cli_common_define.h"

// Constants
#define PIPE_NAME "/tmp/oam_pipe"
#define PIPE_NAME_RESP "/tmp/oam_pipe_resp"
#define BUFFER_SIZE 4096
#define LOG_FILE "/tmp/cli_out.txt"
#define REPLY_COUNT 4

// Function prototypes
void oam_handle_debug_req_info(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_banner_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_inventory_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_system_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_write_memory_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_hostname_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_no_hostname_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_factory_defaults_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_save_config_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_restore_config_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_login_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_write_default_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_hostname_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_clock_set_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_clock_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_cpuload_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_memory_usage_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_cpuload_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_memory_usage_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_danger_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_high_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_low_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_temperature_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_system_software_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_system_image_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_system_show_sysrepo_info_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_sync_show_ptp_status_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_sync_show_sync_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_sync_mode_ptp_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_sync_mode_synce_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_sync_mode_freerun_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_sync_show_synce_status_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_cplane_show_oran_atten_status_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_cplane_show_pwr_status_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_cplane_show_port_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_cplane_power_amp_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_cplane_tx_dpd_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_cplane_show_oran_prach_cplane_info_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_vlan_range_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_vlan_show_all_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_vlan_show_brief_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_iface_show_sfp_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_ecpri_ul_mtu_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_ecpri_mac_destination_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_show_oran_ecpri_info_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_ecpri_vlan_control_enable_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_ecpri_vlan_control_disable_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_ecpri_vlan_id_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_oran_port_trunk_allowed_vlan_add_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_show_interface_brief_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_show_interface_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_ip_address_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_no_ip_address_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_dhcp_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_no_dhcp_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_iface_show_ip_interface_brief_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_user_show_user_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_user_add_user_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_user_del_user_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_user_change_password_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_kpi_show_statistics_req(struct cli *cli, oru_general_msg_t* msg);
int32_t oam_handle_kpi_clear_statistics_req(struct cli *cli, oru_general_msg_t* msg);

int32_t oam_handle_debug_cmd_test_req(struct cli *cli, oru_general_msg_t* msg);

// request handle
typedef int32_t (*ReqParser)(struct cli* cli, oru_general_msg_t* req);
typedef struct {
    uint16_t func_id;
    ReqParser req_parser;
} cmd_list_t;

// Command list
static cmd_list_t cmd_list[ORU_MSG_TYPE_NUM][MAX_CMD_NUM] = {
    [ORU_MSG_TYPE_SYSTEM] = {
        {ORU_CMD_SYSTEM_SHOW_BANNER, oam_handle_system_show_banner_req},
        {ORU_CMD_SYSTEM_SHOW_INVENTORY, oam_handle_system_show_inventory_req},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_INFO, oam_handle_system_show_system_info_req},
        {ORU_CMD_SYSTEM_WRITE_MEMORY, oam_handle_system_write_memory_req},
        {ORU_CMD_SYSTEM_HOSTNAME, oam_handle_system_hostname_req},
        {ORU_CMD_SYSTEM_NO_HOSTNAME, oam_handle_system_no_hostname_req},
        {ORU_CMD_SYSTEM_FACTORY_DEFAULTS, oam_handle_system_factory_defaults_req},
        {ORU_CMD_SYSTEM_SAVE_CONFIG, oam_handle_system_save_config_req},
        {ORU_CMD_SYSTEM_RESTORE_CONFIG, oam_handle_system_restore_config_req},
        {ORU_CMD_SYSTEM_LOGIN, oam_handle_system_login_req},
        {ORU_CMD_SYSTEM_WRITE_DEFAULT, oam_handle_system_write_default_req},
        {ORU_CMD_SYSTEM_SHOW_HOSTNAME, oam_handle_system_show_hostname_req},
        {ORU_CMD_SYSTEM_CLOCK_SET, oam_handle_system_clock_set_req},
        {ORU_CMD_SYSTEM_SHOW_CLOCK, oam_handle_system_show_clock_req},
        {ORU_CMD_SYSTEM_CPULOAD_THRESHOLD, oam_handle_system_cpuload_threshold_req},
        {ORU_CMD_SYSTEM_MEMORY_USAGE_THRESHOLD, oam_handle_system_memory_usage_threshold_req},
        {ORU_CMD_SYSTEM_SHOW_CPULOAD, oam_handle_system_show_cpuload_req},
        {ORU_CMD_SYSTEM_SHOW_MEMORY_USAGE, oam_handle_system_show_memory_usage_req},
        {ORU_CMD_SYSTEM_DANGER_TEMPERATURE_THRESHOLD, oam_handle_system_danger_temperature_threshold_req},
        {ORU_CMD_SYSTEM_HIGH_HIGH_TEMPERATURE_THRESHOLD, oam_handle_system_high_high_temperature_threshold_req},
        {ORU_CMD_SYSTEM_HIGH_TEMPERATURE_THRESHOLD, oam_handle_system_high_temperature_threshold_req},
        {ORU_CMD_SYSTEM_LOW_TEMPERATURE_THRESHOLD, oam_handle_system_low_temperature_threshold_req},
        {ORU_CMD_SYSTEM_SHOW_TEMPERATURE, oam_handle_system_show_temperature_req},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_SOFTWARE_INFO, oam_handle_system_show_system_software_info_req},
        {ORU_CMD_SYSTEM_SHOW_SYSTEM_IMAGE_INFO, oam_handle_system_show_system_image_info_req},
        {ORU_CMD_SYSTEM_SHOW_SYSREPO_INFO, oam_handle_system_show_sysrepo_info_req},
    },
    [ORU_MSG_TYPE_SYNC] = {
        {ORU_CMD_SYNC_SHOW_PTP_STATUS, oam_handle_sync_show_ptp_status_req},
        {ORU_CMD_SYNC_SHOW_SYNC_INFO, oam_handle_sync_show_sync_info_req},
        {ORU_CMD_SYNC_MODE_PTP, oam_handle_sync_mode_ptp_req},
        {ORU_CMD_SYNC_MODE_SYNC_E, oam_handle_sync_mode_synce_req},
        {ORU_CMD_SYNC_MODE_FREERUN, oam_handle_sync_mode_freerun_req},
        {ORU_CMD_SYNC_SHOW_SYNCE_STATUS, oam_handle_sync_show_synce_status_req},
    },
    [ORU_MSG_TYPE_CPLANE] = {
        {ORU_CMD_CPLANE_SHOW_ORAN_ATTEN_STATUS, oam_handle_cplane_show_oran_atten_status_req},
        {ORU_CMD_CPLANE_SHOW_PWR_STATUS, oam_handle_cplane_show_pwr_status_req},
        {ORU_CMD_CPLANE_SHOW_PORT_INFO, oam_handle_cplane_show_port_info_req},
        {ORU_CMD_CPLANE_POWER_AMP, oam_handle_cplane_power_amp_req},
        {ORU_CMD_CPLANE_TX_DPD, oam_handle_cplane_tx_dpd_req},
        {ORU_CMD_CPLANE_SHOW_ORAN_PRACH_CPLANE_INFO, oam_handle_cplane_show_oran_prach_cplane_info_req},
    },
    [ORU_MSG_TYPE_VLAN] = {
        {ORU_CMD_VLAN_RANGE, oam_handle_vlan_range_req},
        {ORU_CMD_VLAN_SHOW_ALL, oam_handle_vlan_show_all_req},
        {ORU_CMD_VLAN_SHOW_BRIEF, oam_handle_vlan_show_brief_req},        
    },
    [ORU_MSG_TYPE_IFACE] = {
        {ORU_CMD_IFACE_SHOW_SFP, oam_handle_iface_show_sfp_req},
        {ORU_CMD_IFACE_ORAN_ECPRI_UL_MTU, oam_handle_iface_oran_ecpri_ul_mtu_req},
        {ORU_CMD_IFACE_ORAN_ECPRI_MAC_DESTINATION, oam_handle_iface_oran_ecpri_mac_destination_req},
        {ORU_CMD_IFACE_SHOW_ORAN_ECPRI_INFO, oam_handle_iface_show_oran_ecpri_info_req},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_ENABLE, oam_handle_iface_oran_ecpri_vlan_control_enable_req},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_CONTROL_DISABLE, oam_handle_iface_oran_ecpri_vlan_control_disable_req},
        {ORU_CMD_IFACE_ORAN_ECPRI_VLAN_ID, oam_handle_iface_oran_ecpri_vlan_id_req},
        {ORU_CMD_IFACE_ORAN_PORT_TRUNK_ALLOWED_VLAN_ADD, oam_handle_iface_oran_port_trunk_allowed_vlan_add_req},
        {ORU_CMD_IFACE_SHOW_INTERFACE_BRIEF, oam_handle_iface_show_interface_brief_req},
        {ORU_CMD_IFACE_SHOW_INTERFACE, oam_handle_iface_show_interface_req},
        {ORU_CMD_IFACE_IP_ADDRESS, oam_handle_iface_ip_address_req},
        {ORU_CMD_IFACE_NO_IP_ADDRESS, oam_handle_iface_no_ip_address_req},
        {ORU_CMD_IFACE_DHCP, oam_handle_iface_dhcp_req},
        {ORU_CMD_IFACE_NO_DHCP, oam_handle_iface_no_dhcp_req},
        {ORU_CMD_IFACE_SHOW_IP_INTERFACE_BRIEF, oam_handle_iface_show_ip_interface_brief_req},
    },
    [ORU_MSG_TYPE_USER] = {
        {ORU_CMD_USER_SHOW_USER, oam_handle_user_show_user_req},
        {ORU_CMD_USER_ADD_USER, oam_handle_user_add_user_req},
        {ORU_CMD_USER_DEL_USER, oam_handle_user_del_user_req},
        {ORU_CMD_USER_CHANGE_PASSWORD, oam_handle_user_change_password_req},
    },
    [ORU_MSG_TYPE_KPI] = {
        {ORU_CMD_KPI_SHOW_STATISTICS, oam_handle_kpi_show_statistics_req},
        {ORU_CMD_KPI_CLEAR_STATISTICS, oam_handle_kpi_clear_statistics_req},
    },
    [ORU_MSG_TYPE_DEBUG] = {
        {ORU_CMD_DEBUG_CMD_TEST, oam_handle_debug_cmd_test_req},
    }
};

// complement function
void log_error(const char *message) {
    perror(message);  // Log the error with a description of the problem
}

bool create_fifo_if_needed(const char *fifo_name) {
    if (access(fifo_name, F_OK) == -1) {
        if (mkfifo(fifo_name, 0666) == -1) {
            log_error("mkfifo failed");
            return false;
        }
    }
    return true; 
}

ssize_t read_from_fifo(int fd, char *buffer, size_t size) {
    ssize_t bytesRead = read(fd, buffer, size);
    if (bytesRead == -1) {
        log_error("read failed");
        return -1;  
    }
    return bytesRead;
}

bool write_to_fifo(int fd, const char *message) {
    if (write(fd, message, strlen(message) + 1) == -1) {
        log_error("write failed");
        return false;
    }
    return true;
}

void write_to_log(const char *message) {
    FILE *file = fopen(LOG_FILE, "a");
    if (!file) {
        log_error("fopen failed");
        return;
    }

    time_t now = time(NULL);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // fprintf(file, "[%s] %s", time_str, message);
    fprintf(file, "%s", message);
    fclose(file);
}

// CLI output function
void cli_out(struct cli *cli, const char *format, ...) {
    char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, BUFFER_SIZE, format, args);
    va_end(args);
    
    write(cli->sock, buffer, strlen(buffer) + 1);
    usleep(1); 
    // write_to_log(buffer);  // Optional log to file
}


// DISPLAY functions 
// Dump received data in hexadecimal format
void dump_data(const char *buffer, size_t length) {
    printf("Data dump:\n");
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
} 

void oam_handle_debug_req_info(struct cli *cli, oru_general_msg_t* req)
{
    printf("Debug: CLI command: %s\n", cli->cmd);
    printf("Debug: Request message type: %d\n", req->header.msg_type);
    printf("Debug: Request function ID: %d\n", req->header.func_id);
    printf("Debug: Request message size: %d\n", req->header.msg_size);

    cli = cli;
}

void display_file_content(struct cli* cli, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        log_error("Failed to open file");
        return;  // Just return, don't exit on error
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        cli_out(cli, line);
        if (errno) {
            log_error("Failed to send line to CLI");
            break;  // Stop reading if there's an error sending to CLI
        }
    }

    if (ferror(file)) {
        log_error("Error reading file");
    }

    fclose(file);
}

// Implementations of the request handler functions
int32_t oam_handle_system_show_banner_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_inventory_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_system_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_write_memory_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_hostname_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_no_hostname_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_factory_defaults_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_save_config_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_restore_config_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_login_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_write_default_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_hostname_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_clock_set_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_clock_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_cpuload_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_memory_usage_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_cpuload_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_memory_usage_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_danger_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_high_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_high_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_low_temperature_threshold_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_temperature_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_system_software_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_system_image_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_system_show_sysrepo_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_show_ptp_status_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_show_sync_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_mode_ptp_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_mode_synce_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_mode_freerun_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_sync_show_synce_status_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_show_oran_atten_status_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_show_pwr_status_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_show_port_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_power_amp_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_tx_dpd_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_cplane_show_oran_prach_cplane_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_vlan_range_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_vlan_show_all_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_vlan_show_brief_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_show_sfp_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_ecpri_ul_mtu_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_ecpri_mac_destination_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_show_oran_ecpri_info_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_ecpri_vlan_control_enable_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_ecpri_vlan_control_disable_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_ecpri_vlan_id_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_oran_port_trunk_allowed_vlan_add_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_show_interface_brief_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_show_interface_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_ip_address_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_no_ip_address_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_dhcp_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_no_dhcp_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_iface_show_ip_interface_brief_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_user_show_user_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_user_add_user_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_user_del_user_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_user_change_password_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_kpi_show_statistics_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_kpi_clear_statistics_req(struct cli *cli, oru_general_msg_t* msg) {
    oam_handle_debug_req_info(cli, msg);
    return 0;
}

int32_t oam_handle_debug_cmd_test_req(struct cli *cli, oru_general_msg_t *msg)
{
    char *replies[] = {
        "Reply 1 from reader process!\n",
        "Reply 2 from reader process!\n",
        "Reply 3 from reader process!\n",
        "Reply 4 from reader process!\n"
    };

    oam_handle_debug_req_info(cli, msg);

    // Write multiple replies to FIFO2
    for (int i = 0; i < REPLY_COUNT; i++)
    {
        printf("Sending: %s", replies[i]);
        if (!write_to_fifo(cli->sock, replies[i]))
        {
            return -1;
        }

        usleep(1000);
    }

    // Write file content to FIFO2
    const char *file_path = "/home/user/projects/branches/master/pc805-mplane/sources/oam_netconf_1.0.0/usecases/pc805/uc2/du_config_req.json";

    display_file_content(cli, file_path);

    return 0;
}

// oam_oru_ci_server_handle
int32_t oam_handle_request(struct cli *cli, oru_general_msg_t* req) {
    oru_msg_type_e msg_type = req->header.msg_type;
    if (msg_type >= ORU_MSG_TYPE_NUM) {
        cli_out(cli, "Error: Invalid msg_type = %d\n", msg_type);
        return -1;
    }

    // cli_out(cli, "Handling request for msg_type = %d\n", msg_type);

    uint16_t func_id = req->header.func_id;
    for (int i = 0; i < MAX_CMD_NUM; i++) {
        // printf("Checking func_id = %d against cmd_list[%d][%d].func_id = %d\n", func_id, msg_type, i, cmd_list[msg_type][i].func_id);
        if (func_id == cmd_list[msg_type][i].func_id) {
            // printf("Found matching func_id = %d, calling req_parser\n", func_id);
            return cmd_list[msg_type][i].req_parser(cli, req);
        }
    }

    cli_out(cli, "Error: Request handler not found for func_id = %d\n", func_id);

    return -1;
}

// Unpack the request data from the buffer
bool unpack_data_from_buffer(const char *buffer, const char *key, oru_general_msg_t *req) {
    if (!buffer || !key || !req) {
        return false;
    }

    uint16_t cmd_len = strlen(key);
    if (strncmp(buffer, key, cmd_len) != 0) {
        return false;
    }

    const char *req_data = buffer + cmd_len;
    const oru_msg_header_t *req_header = (const oru_msg_header_t *)req_data;

    if (req_header->msg_size > BUFFER_SIZE - cmd_len) {
        printf("Debug: Invalid message size.\n");
        return false;
    }

    memcpy(req, req_data, req_header->msg_size);
    return true;
}

// Function to handle a request
bool oam_handle_cli_request(const char *buffer, const char *fifo2, oru_general_msg_t *req) {
    int fd2 = open(fifo2, O_WRONLY);
    if (fd2 == -1) {
        log_error("open fifo2 failed");
        return false;
    }

    if (!unpack_data_from_buffer(buffer, "cmd ", req)) {
        printf("Failed to unpack data.\n");
        close(fd2);
        return false;
    }

    printf("Unpacked data successfully.\n");
    printf("Msg type: %d\n", req->header.msg_type);
    printf("Function ID: %d\n", req->header.func_id);
    printf("Message size: %d\n", req->header.msg_size);

    struct cli cli = {0};
    cli.sock = fd2;

    if (oam_handle_request(&cli, req) != 0) {
        printf("Failed to handle request.\n");
        close(fd2);
        return false;
    }

    // Write the end of message to FIFO2
    if (!write_to_fifo(fd2, "END OF MESSAGE\n")) {
        printf("Failed to handle endofmessage.\n");
        close(fd2);
        return false;
    }

    close(fd2);
    return true;
}

int main() {
    char buffer[BUFFER_SIZE];

    // Ensure FIFOs exist
    if (!create_fifo_if_needed(PIPE_NAME) || !create_fifo_if_needed(PIPE_NAME_RESP)) {
        return -1;
    }

    // Open FIFO1 for read only
    int fd1 = open(PIPE_NAME, O_RDONLY);
    if (fd1 == -1) {
        log_error("open fifo1 failed");
        return -1;
    }

    // Read the message from FIFO1
    ssize_t bytesRead = read_from_fifo(fd1, buffer, sizeof(buffer));
    close(fd1);
    if (bytesRead == -1) {
        return -1;
    }

    printf("Received: %s\n", buffer);
    printf("Number of bytes received: %zd\n", bytesRead);

    // Initialize request structure
    oru_general_msg_t req;
    if (!oam_handle_cli_request(buffer, PIPE_NAME_RESP, &req)) {
        return -1;  // Return failure if handling the request fails
    }

    return 0;
}
