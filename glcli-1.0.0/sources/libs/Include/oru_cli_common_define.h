/*
 * GIGALANE PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with GIGALANE and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright GIGALANE.
 */

#ifndef ORU_CLI_COMMON_DEFINED_H
#define ORU_CLI_COMMON_DEFINED_H

#include <stdint.h>

// Constants
#define BUFFER_SIZE 4096
#define SERVER_IP "127.0.0.1"  // Change this to the server's IP address if needed
#define SERVER_PORT 8090

#define MAX_IFNAME_LEN 128
#define MAX_CMD_OUTPUT_LEN 1024

// PIPE defined
#define PIPE_NAME "/tmp/oam_pipe"
#define PIPE_NAME_RESP "/tmp/oam_pipe_resp"

// Decclare the mutex with OAM_PIPE_LOCK and OAM_PIPE_UNLOCK
extern pthread_mutex_t oam_pipe_mutex;
#define OAM_PIPE_LOCK pthread_mutex_lock(&oam_pipe_mutex);
#define OAM_PIPE_UNLOCK pthread_mutex_unlock(&oam_pipe_mutex);

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
    ORU_CMD_SYSTEM_SHOW_SYSREPO_INFO,
    ORU_CMD_SYSTEM_NUM = ORU_CMD_SYSTEM_SHOW_SYSREPO_INFO
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

// Debug command
typedef enum {
    ORU_CMD_DEBUG_CMD_TEST = 1,
    ORU_CMD_DEBUG_NUM = ORU_CMD_DEBUG_CMD_TEST
} oru_cmd_debug_e;

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
    ORU_MSG_TYPE_DEBUG,
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

// ip address configuration 
typedef struct {
    char ip[32];
    char iface[32];
    int mask;
} oru_iface_ip_address_req_t;

typedef struct {
    int32_t error_status;
} oru_iface_ip_address_resp_t;

// no ip address configuration 
typedef struct {
    char ip[32];
    char iface[32];
    int mask;
} oru_iface_no_ip_address_req_t;

typedef struct {
    int32_t error_status;
} oru_iface_no_ip_address_resp_t;


// dhcp configuration
typedef struct {
    char iface[32];
} oru_iface_dhcp_req_t;

typedef struct {
    int32_t error_status;
} oru_iface_dhcp_resp_t;

// no dhcp configuration
typedef struct {
    char iface[32];
} oru_iface_no_dhcp_req_t;

typedef struct {
    int32_t error_status;
} oru_iface_no_dhcp_resp_t;


// vlan command msg type

// sync command msg type

// cplane command msg type

// User command msg type

// KPI command msg type


#endif // ORU_CLI_COMMON_DEFINED_H