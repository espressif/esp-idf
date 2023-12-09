/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_DF_MODEL_API_H_
#define _ESP_BLE_MESH_DF_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_GET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7B)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_SET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7C)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7D)
#define ESP_BLE_MESH_MODEL_OP_PATH_METRIC_GET                       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7E)
#define ESP_BLE_MESH_MODEL_OP_PATH_METRIC_SET                       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7F)
#define ESP_BLE_MESH_MODEL_OP_PATH_METRIC_STATUS                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x80)
#define ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_GET              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x81)
#define ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_SET              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x82)
#define ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x83)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ADD                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x84)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEL                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x85)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x86)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_ADD             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x87)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_DEL             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x88)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x89)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8A)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8B)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_GET      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8C)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_STATUS   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8D)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8E)
#define ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_STATUS       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x8F)
#define ESP_BLE_MESH_MODEL_OP_WANTED_LANES_GET                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x90)
#define ESP_BLE_MESH_MODEL_OP_WANTED_LANES_SET                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x91)
#define ESP_BLE_MESH_MODEL_OP_WANTED_LANES_STATUS                   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x92)
#define ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_GET                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x93)
#define ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_SET                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x94)
#define ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_STATUS                   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x95)
#define ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_GET                ESP_BLE_MESH_MODEL_OP_2(0x80, 0x96)
#define ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_SET                ESP_BLE_MESH_MODEL_OP_2(0x80, 0x97)
#define ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_STATUS             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x98)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_GET             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x99)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_SET             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9A)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9B)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_GET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9C)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_SET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9D)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9E)
#define ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_GET                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x9F)
#define ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_SET                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA0)
#define ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_STATUS                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA1)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_PATHS_GET                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA2)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_PATHS_STATUS                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA3)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_GET               ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA4)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_SET               ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA5)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_STATUS            ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA6)
#define ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_GET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA7)
#define ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_SET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA8)
#define ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x80, 0xA9)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_GET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xAB)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_SET         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xAC)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x80, 0xAD)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_GET     ESP_BLE_MESH_MODEL_OP_2(0x80, 0xAE)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_SET     ESP_BLE_MESH_MODEL_OP_2(0x80, 0xAF)
#define ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_STATUS  ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB0)

#define ESP_BLE_MESH_PATH_DISC_INTERVAL_5_SEC                       0
#define ESP_BLE_MESH_PATH_DISC_INTERVAL_30_SEC                      1

#define ESP_BLE_MESH_LANE_DISC_GUARD_INTERVAL_2_SEC                 0
#define ESP_BLE_MESH_LANE_DISC_GUARD_INTERVAL_10_SEC                1

#define ESP_BLE_MESH_DIRECTED_PUB_POLICY_MANAGED_FLOODING           0x00
#define ESP_BLE_MESH_DIRECTED_PUB_POLICY_DIRECTED_FORWARDING        0x01

#define ESP_BLE_MESH_GET_FILTER_MASK(fp, nfp, pom, dm) \
        (((dm) << 3) | ((pom) << 2) | ((nfp) << 1) | (fp))

/** @def    ESP_BLE_MESH_MODEL_DF_SRV
 *
 *  @brief  Define a new Directed Forwarding Configuration Server model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  srv_data Pointer to a unique Directed Forwarding Configuration
 *                   Server model user_data.
 *
 *  @return New Directed Forwarding Configuration Server model instance.
 */
#define ESP_BLE_MESH_MODEL_DF_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DF_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_DF_CLI
 *
 *  @brief  Define a new Directed Forwarding Configuration Client model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique Directed Forwarding Configuration
 *                   Client model user_data.
 *
 *  @return New Directed Forwarding Configuration Client model instance.
 */
#define ESP_BLE_MESH_MODEL_DF_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DF_CLI, \
                               NULL, NULL, cli_data)

/** Directed Forwarding Configuration Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to Directed Forwarding Configuration Server model */

    uint8_t  directed_net_transmit;                 /*!< Directed Network Transmit state */
    uint8_t  directed_relay_retransmit;             /*!< Directed Relay Retransmit state */

    int8_t   default_rssi_threshold;                /*!< Default RSSI Threshold state */
    uint8_t  rssi_margin;                           /*!< RSSI Margin state */

    uint16_t directed_node_paths;                   /*!< Directed Node Paths state */
    uint16_t directed_relay_paths;                  /*!< Directed Relay Paths state */
    uint16_t directed_proxy_paths;                  /*!< Directed Proxy Paths state */
    uint16_t directed_friend_paths;                 /*!< Directed Friend Paths state */

    uint16_t path_monitor_interval;                 /*!< Path Monitoring Interval state */
    uint16_t path_disc_retry_interval;              /*!< Path Discovery Retry Interval state */
    uint8_t  path_disc_interval:1,                  /*!< Path Discovery Interval state */
             lane_disc_guard_interval:1;            /*!< Lane Discovery Guard Interval state */

    uint8_t  directed_ctl_net_transmit;             /*!< Directed Control Network Transmit state */
    uint8_t  directed_ctl_relay_retransmit;         /*!< Directed Control Relay Retransmit state */
} esp_ble_mesh_df_srv_t;

/** Parameters of Directed Control Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_directed_control_get_t;

/** Parameters of Directed Control Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  directed_forwarding;                   /*!< New Directed Forwarding state */
    uint8_t  directed_relay;                        /*!< New Directed Relay state */
    uint8_t  directed_proxy;                        /*!< New Directed Proxy state */
    uint8_t  directed_proxy_use_default;            /*!< New Directed Proxy Use Directed Default state or value to ignore */
    uint8_t  directed_friend;                       /*!< New Directed Friend state or value to ignore */
} esp_ble_mesh_directed_control_set_t;

/** Parameters of Path Metric Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_path_metric_get_t;

/** Parameters of Path Metric Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  path_metric_type:3,                    /*!< New Path Metric Type state */
             path_lifetime:2;                       /*!< New Path Lifetime state */
} esp_ble_mesh_path_metric_set_t;

/** Parameters of Discovery Table Capabilities Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_discovery_table_caps_get_t;

/** Parameters of Discovery Table Capabilities Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  max_concurr_init;                      /*!< New Max Concurrent Init state */
} esp_ble_mesh_discovery_table_caps_set_t;

/** Parameters of Forwarding Table Add. */
typedef struct {
    uint16_t net_idx:12,                            /*!< NetKey Index */
             unicast_dst:1,                         /*!< Indicates whether or not the destination of the path is a unicast address */
             bw_path_validated:1;                   /*!< Indicates whether or not the backward path has been validated */
    esp_ble_mesh_uar_t path_origin;                 /*!< Unicast address range of the Path Origin */
    /** Path target address */
    union {
        esp_ble_mesh_uar_t path_target;             /*!< Unicast address range of the Path Target */
        uint16_t multicast_dst;                     /*!< Multicast destination address */
    };
    uint16_t bearer_twd_path_origin;                /*!< Index of the bearer toward the Path Origin */
    uint16_t bearer_twd_path_target;                /*!< Index of the bearer toward the Path Target */
} esp_ble_mesh_forwarding_table_add_t;              /*!< Parameters of Forwarding Table Add */

/** Parameters of Forwarding Table Delete. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */
} esp_ble_mesh_forwarding_table_delete_t;

/** Parameters of Forwarding Table Dependents Add. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */
    uint8_t  dep_origin_uar_list_size;              /*!< Number of entries in the Dependent_Origin_Unicast_Addr_Range_List field */
    uint8_t  dep_target_uar_list_size;              /*!< Number of entries in the Dependent_Target_Unicast_Addr_Range_List field */
    esp_ble_mesh_uar_t *dep_origin_uar_list;        /*!< List of the unicast address ranges of the dependent nodes of the Path Origin */
    esp_ble_mesh_uar_t *dep_target_uar_list;        /*!< List of the unicast address ranges of the dependent nodes of the Path Target */
} esp_ble_mesh_forwarding_table_deps_add_t;

/** Parameters of Forwarding Table Dependents Delete. */
typedef struct {
    uint16_t  net_idx;                              /*!< NetKey Index */
    uint16_t  path_origin;                          /*!< Primary element address of the Path Origin */
    uint16_t  dst;                                  /*!< Destination address */
    uint8_t   dep_origin_list_size;                 /*!< Number of entries in the Dependent_Origin_List field */
    uint8_t   dep_target_list_size;                 /*!< Number of entries in the Dependent_Target_List field */
    uint16_t *dep_origin_list;                      /*!< List of the primary element addresses of the dependent nodes of the Path Origin */
    uint16_t *dep_target_list;                      /*!< List of the primary element addresses of the dependent nodes of the Path Target */
} esp_ble_mesh_forwarding_table_deps_delete_t;

/** Parameters of Forwarding Table Entries Count Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_forwarding_table_entries_cnt_get_t;

/** Parameters of Forwarding Table Entries Get. */
typedef struct {
    uint16_t net_idx:12,                            /*!< NetKey Index */
             filter_mask:4;                         /*!< Filter to be applied to the Forwarding Table entries */
    uint16_t start_index;                           /*!< Start offset to read in units of Forwarding Table entries */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */

    bool     include_id;                            /*!< Indicate whether or not the Forwarding Table Update Identifier is present */
    uint16_t update_id;                             /*!< Last saved Forwarding Table Update Identifier (Optional) */
} esp_ble_mesh_forwarding_table_entries_get_t;

/** Parameters of Forwarding Table Dependents Get. */
typedef struct {
    uint16_t net_idx:12,                            /*!< NetKey Index */
             dep_list_mask:2,                       /*!< Filter applied to the lists of unicast address ranges for dependent nodes */
             fixed_path_flag:1;                     /*!< Indicate whether or not to return the unicast address ranges of dependent nodes in a fixed path entry */
    uint16_t start_index;                           /*!< Start offset in units of unicast address ranges */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */

    bool     include_id;                            /*!< Indicate whether or not the Forwarding Table Update Identifier is present */
    uint16_t update_id;                             /*!< Last saved Forwarding Table Update Identifier (Optional) */
} esp_ble_mesh_forwarding_table_deps_get_t;

/** Parameters of Wanted Lanes Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_wanted_lanes_get_t;

/** Parameters of Wanted Lanes Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  wanted_lanes;                          /*!< New Wanted Lanes state */
} esp_ble_mesh_wanted_lanes_set_t;

/** Parameters of Two Way Path Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_two_way_path_get_t;

/** Parameters of Two Way Path Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  two_way_path:1;                        /*!< Two way path flag */
} esp_ble_mesh_two_way_path_set_t;

/** Parameters of Path Echo Interval Get. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
} esp_ble_mesh_path_echo_interval_get_t;

/** Parameters of Path Echo Interval Set. */
typedef struct {
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  unicast_echo_interval;                 /*!< New Unicast Echo Interval state or indication of no state change */
    uint8_t  multicast_echo_interval;               /*!< New Multicast Echo Interval state or indication of no state change */
} esp_ble_mesh_path_echo_interval_set_t;

/** Parameters of Directed Network Transmit Set. */
typedef struct {
    uint8_t net_transmit;                           /*!< New Directed Network Transmit state */
} esp_ble_mesh_directed_net_transmit_set_t;

/** Parameters of Directed Relay Retransmit Set. */
typedef struct {
    uint8_t relay_retransmit;                       /*!< New Directed Relay Retransmit state */
} esp_ble_mesh_directed_relay_retransmit_set_t;

/** Parameters of RSSI Threshold Set. */
typedef struct {
    uint8_t rssi_margin;                            /*!< New RSSI Margin state */
} esp_ble_mesh_rssi_threshold_set_t;

/** Parameters of Directed Publish Policy Get. */
typedef struct {
    uint16_t element_addr;                          /*!< Address of the element */
    uint16_t company_id;                            /*!< Company ID */
    uint16_t model_id;                              /*!< Model ID */
} esp_ble_mesh_directed_publish_policy_get_t;

/** Parameters of Directed Publish Policy Set. */
typedef struct {
    uint8_t  direct_pub_policy;                     /*!< New Directed Publish Policy state */
    uint16_t element_addr;                          /*!< Address of the element */
    uint16_t company_id;                            /*!< Company ID */
    uint16_t model_id;                              /*!< Model ID */
} esp_ble_mesh_directed_publish_policy_set_t;

/** Parameters of Path Discovery Timing Control Set. */
typedef struct {
    uint16_t path_monitor_interval;                 /*!< New Path Monitoring Interval state */
    uint16_t path_disc_retry_interval;              /*!< New Path Discovery Retry Interval state */
    uint8_t  path_disc_interval:1,                  /*!< New Path Discovery Interval state */
             lane_disc_guard_interval:1;            /*!< New Lane Discovery Guard Interval state */
} esp_ble_mesh_path_discovery_timing_ctl_set_t;

/** Parameters of Directed Control Network Transmit Set. */
typedef struct {
    uint8_t net_transmit;                           /*!< New Directed Control Network Transmit Count state */
} esp_ble_mesh_directed_ctl_net_transmit_set_t;

/** Parameters of Directed Control Relay Retransmit Set. */
typedef struct {
    uint8_t relay_retransmit;                       /*!< New Directed Control Relay Retransmit Count state */
} esp_ble_mesh_directed_ctl_relay_retransmit_set_t;

/**
 * @brief Directed Forwarding Configuration Client model get message union
 */
typedef union {
    esp_ble_mesh_directed_control_get_t             directed_control_get;             /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_GET */
    esp_ble_mesh_path_metric_get_t                  path_metric_get;                  /*!< For ESP_BLE_MESH_MODEL_OP_PATH_METRIC_GET */
    esp_ble_mesh_discovery_table_caps_get_t         disc_table_caps_get;              /*!< For ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_GET */
    esp_ble_mesh_forwarding_table_entries_cnt_get_t forwarding_table_entries_cnt_get; /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_GET */
    esp_ble_mesh_forwarding_table_entries_get_t     forwarding_table_entries_get;     /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_GET */
    esp_ble_mesh_forwarding_table_deps_get_t        forwarding_table_deps_get;        /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET */
    esp_ble_mesh_wanted_lanes_get_t                 wanted_lanes_get;                 /*!< For ESP_BLE_MESH_MODEL_OP_WANTED_LANES_GET */
    esp_ble_mesh_two_way_path_get_t                 two_way_path_get;                 /*!< For ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_GET */
    esp_ble_mesh_path_echo_interval_get_t           path_echo_interval_get;           /*!< For ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_GET */
    esp_ble_mesh_directed_publish_policy_get_t      directed_pub_policy_get;          /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_GET */
} esp_ble_mesh_df_client_get_t;

/**
 * @brief Directed Forwarding Configuration Client model set message union
 */
typedef union {
    esp_ble_mesh_directed_control_set_t              directed_control_set;              /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_SET */
    esp_ble_mesh_path_metric_set_t                   path_metric_set;                   /*!< For ESP_BLE_MESH_MODEL_OP_PATH_METRIC_SET */
    esp_ble_mesh_discovery_table_caps_set_t          disc_table_caps_set;               /*!< For ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_SET */
    esp_ble_mesh_forwarding_table_add_t              forwarding_table_add;              /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ADD */
    esp_ble_mesh_forwarding_table_delete_t           forwarding_table_del;              /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEL */
    esp_ble_mesh_forwarding_table_deps_add_t         forwarding_table_deps_add;         /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_ADD */
    esp_ble_mesh_forwarding_table_deps_delete_t      forwarding_table_deps_del;         /*!< For ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_DEL */
    esp_ble_mesh_wanted_lanes_set_t                  wanted_lanes_set;                  /*!< For ESP_BLE_MESH_MODEL_OP_WANTED_LANES_SET */
    esp_ble_mesh_two_way_path_set_t                  two_way_path_set;                  /*!< For ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_SET */
    esp_ble_mesh_path_echo_interval_set_t            path_echo_interval_set;            /*!< For ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_SET */
    esp_ble_mesh_directed_net_transmit_set_t         directed_net_transmit_set;         /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_SET */
    esp_ble_mesh_directed_relay_retransmit_set_t     directed_relay_retransmit_set;     /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_SET */
    esp_ble_mesh_rssi_threshold_set_t                rssi_threshold_set;                /*!< For ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_SET */
    esp_ble_mesh_directed_publish_policy_set_t       directed_pub_policy_set;           /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_SET */
    esp_ble_mesh_path_discovery_timing_ctl_set_t     path_disc_timing_ctl_set;          /*!< For ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_SET */
    esp_ble_mesh_directed_ctl_net_transmit_set_t     directed_ctl_net_transmit_set;     /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_SET */
    esp_ble_mesh_directed_ctl_relay_retransmit_set_t directed_ctl_relay_retransmit_set; /*!< For ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_SET */
} esp_ble_mesh_df_client_set_t;

/** Parameters of Directed Control Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  directed_forwarding;                   /*!< Current Directed Forwarding state */
    uint8_t  directed_relay;                        /*!< Current Directed Relay state */
    uint8_t  directed_proxy;                        /*!< Current Directed Proxy state */
    uint8_t  directed_proxy_use_default;            /*!< Current Directed Proxy Use Directed Default state or 0xFF */
    uint8_t  directed_friend;                       /*!< Current Directed Friend state */
} esp_ble_mesh_directed_control_status_t;

/** Parameters of Path Metric Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  path_metric_type:3,                    /*!< Current Path Metric Type state */
             path_lifetime:2;                       /*!< Current Path Lifetime state */
} esp_ble_mesh_path_metric_status_t;

/** Parameters of Discovery Table Capabilities Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint8_t  max_concurr_init;                      /*!< Current Max Concurrent Init state */
    uint8_t  max_disc_entries;                      /*!< Max Discovery Table Entries Count state */
} esp_ble_mesh_discovery_table_caps_status_t;

/** Parameters of Forwarding Table Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */
} esp_ble_mesh_forwarding_table_status_t;

/** Parameters of Forwarding Table Dependent Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint16_t path_origin;                           /*!< Primary element address of the Path Origin */
    uint16_t dst;                                   /*!< Destination address */
} esp_ble_mesh_forwarding_table_deps_status_t;

/** Parameters of Forwarding Table Entries Count Status. */
typedef struct {
    uint8_t  status;                                /*!< Status code for the requesting message */
    uint16_t net_idx;                               /*!< NetKey Index */
    uint16_t update_id;                             /*!< Current Forwarding Table Update Identifier state */
    uint16_t fixed_entry_cnt;                       /*!< Number of fixed path entries in the Forwarding Table */
    uint16_t non_fixed_entry_cnt;                   /*!< Number of non-fixed path entries in the Forwarding Table */
} esp_ble_mesh_forwarding_table_entries_cnt_status_t;

/** Parameters of Forwarding Table Entry. */
typedef struct {
    uint16_t fixed_path_flag:1,                     /*!< Indicates whether the table entry is a fixed path entry or a non-fixed path entry */
             unicast_dst_flag:1,                    /*!< Indicates whether or not the destination of the path is a unicast address */
             bw_path_validated_flag:1,              /*!< Indicates whether or not the backward path has been validated */
             bearer_twd_path_origin_ind:1,          /*!< Indicates the presence or absence of the Bearer_Toward_Path_Origin field */
             bearer_twd_path_target_ind:1,          /*!< Indicates the presence or absence of the Bearer_Toward_Path_Target field */
             dep_origin_list_size_ind:2,            /*!< Indicates the size of the Dependent_Origin_List field */
             dep_target_list_size_ind:2;            /*!< Indicates the size of the Dependent_Target_List field */

    /* Only for non-fixed path entry */
    uint8_t  lane_counter;                          /*!< Number of lanes in the path */
    uint16_t path_remaining_time;                   /*!< Path lifetime remaining */
    uint8_t  path_origin_forward_number;            /*!< Forwarding number of the Path Origin */

    /* For fixed path entry and non-fixed path entry */
    esp_ble_mesh_uar_t path_origin;                 /*!< Path Origin unicast address range */
    uint16_t dep_origin_list_size;                  /*!< Current number of entries in the list of dependent nodes of the Path Origin */
    uint16_t bearer_twd_path_origin;                /*!< Index of the bearer toward the Path Origin */
    esp_ble_mesh_uar_t path_target;                 /*!< Path Target unicast address range */
    uint16_t multicast_dst;                         /*!< Multicast destination address */
    uint16_t dep_target_list_size;                  /*!< Current number of entries in the list of dependent nodes of the Path Target */
    uint16_t bearer_twd_path_target;                /*!< Index of the bearer toward the Path Target */
} esp_ble_mesh_forwarding_table_entry_t;

/** Parameters of Forwarding Table Entries Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint16_t net_idx:12,                                /*!< NetKey Index */
             filter_mask:4;                             /*!< Filter applied to the Forwarding Table entries */
    uint16_t start_index;                               /*!< Start offset in units of Forwarding Table entries */
    uint16_t path_origin;                               /*!< Primary element address of the Path Origin */
    uint16_t dst;                                       /*!< Destination address */

    bool include_id;                                    /*!< Indicate whether or not the Forwarding Table Update Identifier is present */
    uint16_t update_id;                                 /*!< Current Forwarding Table Update Identifier state */

    uint8_t  entry_list_size;                           /*!< Current number of entries in the list of Forwarding Table entries */
    esp_ble_mesh_forwarding_table_entry_t *entry_list;  /*!< List of Forwarding Table entries */
} esp_ble_mesh_forwarding_table_entries_status_t;

/** Parameters of Forwarding Table Dependents Get Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint16_t net_idx:12,                                /*!< NetKey Index */
             dep_list_mask:2,                           /*!< Filter applied to the lists of unicast address ranges for dependent nodes */
             fixed_path_flag:1;                         /*!< Flag indicating whether or not to return the unicast address ranges of dependent nodes in a fixed path entry */
    uint16_t start_index;                               /*!< Start offset in units of unicast address ranges */
    uint16_t path_origin;                               /*!< Primary element address of the Path Origin */
    uint16_t dst;                                       /*!< Destination address */

    bool include_id;                                    /*!< Indicate whether or not the Forwarding Table Update Identifier is present */
    uint16_t update_id;                                 /*!< Current Forwarding Table Update Identifier state */

    uint8_t dep_origin_uar_list_size;                   /*!< Number of unicast address ranges in the Dependent_Origin_Unicast_Addr_Range_List field */
    uint8_t dep_target_uar_list_size;                   /*!< Number of unicast address ranges in the Dependent_Target_Unicast_Addr_Range_List field */
    esp_ble_mesh_uar_t *dep_origin_uar_list;            /*!< List of unicast address ranges of dependent nodes of the Path Origin */
    esp_ble_mesh_uar_t *dep_target_uar_list;            /*!< List of unicast address ranges of dependent nodes of the Path Target */
} esp_ble_mesh_forwarding_table_deps_get_status_t;

/** Parameters of Wanted Lanes Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint16_t net_idx;                                   /*!< NetKey Index */
    uint8_t  wanted_lanes;                              /*!< Current Wanted Lanes state */
} esp_ble_mesh_wanted_lanes_status_t;

/** Parameters of Two Way Path Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint16_t net_idx;                                   /*!< NetKey Index */
    uint8_t  two_way_path;                              /*!< Current Two Way Path state */
} esp_ble_mesh_two_way_path_status_t;

/** Parameters of Path Echo Interval Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint16_t net_idx;                                   /*!< NetKey Index */
    uint8_t  unicast_echo_interval;                     /*!< Current Unicast Echo Interval state */
    uint8_t  multicast_echo_interval;                   /*!< Current Multicast Echo Interval state */
} esp_ble_mesh_path_echo_interval_status_t;

/** Parameters of Directed Network Transmit Status. */
typedef struct {
    uint8_t net_transmit;                               /*!< Current Directed Network Transmit state */
} esp_ble_mesh_directed_net_transmit_status_t;

/** Parameters of Directed Relay Retransmit Status. */
typedef struct {
    uint8_t relay_retransmit;                           /*!< Current Directed Relay Retransmit state */
} esp_ble_mesh_directed_relay_retransmit_status_t;

/** Parameters of RSSI Threshold Status. */
typedef struct {
    uint8_t default_rssi_threshold;                     /*!< Default RSSI Threshold state */
    uint8_t rssi_margin;                                /*!< Current RSSI Margin state */
} esp_ble_mesh_rssi_threshold_status_t;

/** Parameters of Directed Paths Status. */
typedef struct {
    uint16_t directed_node_paths;                       /*!< Directed Node Paths state */
    uint16_t directed_relay_paths;                      /*!< Directed Relay Paths state */
    uint16_t directed_proxy_paths;                      /*!< Directed Proxy Paths state */
    uint16_t directed_friend_paths;                     /*!< Directed Friend Paths state */
} esp_ble_mesh_directed_paths_status_t;

/** Parameters of Directed Publish Policy Status. */
typedef struct {
    uint8_t  status;                                    /*!< Status code for the requesting message */
    uint8_t  directed_pub_policy;                       /*!< Current Directed Publish Policy state */
    uint16_t element_addr;                              /*!< Address of the element */
    uint16_t company_id;                                /*!< Company ID */
    uint16_t model_id;                                  /*!< Model ID */
} esp_ble_mesh_directed_pub_policy_status_t;

/** Parameters of Path Discovery Timing Control Status. */
typedef struct {
    uint16_t path_monitor_interval;                     /*!< Current Path Monitoring Interval state */
    uint16_t path_disc_retry_interval;                  /*!< Current Path Discovery Retry Interval state */
    uint8_t  path_disc_interval:1,                      /*!< Current Path Discovery Interval state */
             lane_disc_guard_interval:1;                /*!< Current Lane Discovery Guard Interval state */
} esp_ble_mesh_path_disc_timing_ctl_status_cb_t;

/** Parameters of Directed Control Network Transmit Status. */
typedef struct {
    uint8_t net_transmit;                               /*!< Current Directed Control Network Transmit state */
} esp_ble_mesh_directed_ctl_net_transmit_status_t;

/** Parameters of Directed Control Relay Retransmit Status. */
typedef struct {
    uint8_t relay_retransmit;                           /*!< Current Directed Control Relay Retransmit state */
} esp_ble_mesh_directed_ctl_relay_retransmit_status_t;

/** Result of sending Directed Forwarding Configuration Client messages */
typedef struct {
    int err_code;                                       /*!< Result of sending a message */
} esp_ble_mesh_df_client_send_cb_t;

/**
 * @brief Directed Forwarding Configuration Client model received message union
 */
typedef union {
    esp_ble_mesh_directed_control_status_t              directed_control_status;              /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_STATUS */
    esp_ble_mesh_path_metric_status_t                   path_metric_status;                   /*!< ESP_BLE_MESH_MODEL_OP_PATH_METRIC_STATUS */
    esp_ble_mesh_discovery_table_caps_status_t          disc_table_caps_status;               /*!< ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_STATUS */
    esp_ble_mesh_forwarding_table_status_t              forwarding_table_status;              /*!< ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_STATUS */
    esp_ble_mesh_forwarding_table_deps_status_t         forwarding_table_deps_status;         /*!< ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_STATUS */
    esp_ble_mesh_forwarding_table_entries_cnt_status_t  forwarding_table_entries_cnt_status;  /*!< ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_CNT_STATUS */
    esp_ble_mesh_forwarding_table_entries_status_t      forwarding_table_entries_status;      /*!< ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_ENTRIES_STATUS */
    esp_ble_mesh_forwarding_table_deps_get_status_t     forwarding_table_deps_get_status;     /*!< ESP_BLE_MESH_MODEL_OP_FORWARDING_TABLE_DEPS_GET_STATUS */
    esp_ble_mesh_wanted_lanes_status_t                  wanted_lanes_status;                  /*!< ESP_BLE_MESH_MODEL_OP_WANTED_LANES_STATUS */
    esp_ble_mesh_two_way_path_status_t                  two_way_path_status;                  /*!< ESP_BLE_MESH_MODEL_OP_TWO_WAY_PATH_STATUS */
    esp_ble_mesh_path_echo_interval_status_t            path_echo_interval_status;            /*!< ESP_BLE_MESH_MODEL_OP_PATH_ECHO_INTERVAL_STATUS */
    esp_ble_mesh_directed_net_transmit_status_t         directed_net_transmit_status;         /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_NET_TRANSMIT_STATUS */
    esp_ble_mesh_directed_relay_retransmit_status_t     directed_relay_retransmit_status;     /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_RELAY_RETRANSMIT_STATUS */
    esp_ble_mesh_rssi_threshold_status_t                rssi_threshold_status;                /*!< ESP_BLE_MESH_MODEL_OP_RSSI_THRESHOLD_STATUS */
    esp_ble_mesh_directed_paths_status_t                directed_paths_status;                /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_PATHS_STATUS */
    esp_ble_mesh_directed_pub_policy_status_t           directed_pub_policy_status;           /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_PUB_POLICY_STATUS */
    esp_ble_mesh_path_disc_timing_ctl_status_cb_t       path_disc_timing_ctl_status;          /*!< ESP_BLE_MESH_MODEL_OP_PATH_DISCOVERY_TIMING_CTL_STATUS */
    esp_ble_mesh_directed_ctl_net_transmit_status_t     directed_ctl_net_transmit_status;     /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_NET_TRANSMIT_STATUS */
    esp_ble_mesh_directed_ctl_relay_retransmit_status_t directed_ctl_relay_retransmit_status; /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_CTL_RELAY_RETRANSMIT_STATUS */
} esp_ble_mesh_df_client_recv_cb_t;

/** Directed Forwarding Configuration Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params;         /*!< Client common parameters, used by all events. */
    /** Union of DF Client callback */
    union {
        esp_ble_mesh_df_client_send_cb_t send;          /*!< Result of sending a message */
        esp_ble_mesh_df_client_recv_cb_t recv;          /*!< Parameters of received status message */
    };
} esp_ble_mesh_df_client_cb_param_t;

/** This enum value is the event of Directed Forwarding Configuration Client model */
typedef enum {
    ESP_BLE_MESH_DF_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_DF_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_DF_CLIENT_RECV_GET_RSP_EVT,
    ESP_BLE_MESH_DF_CLIENT_RECV_SET_RSP_EVT,
    ESP_BLE_MESH_DF_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_DF_CLIENT_EVT_MAX,
} esp_ble_mesh_df_client_cb_event_t;

/**
 * @brief Directed Forwarding Configuration Server model related context.
 */

/**
 * @brief Directed Forwarding Configuration Server model state change value union
 */
typedef union {
    uint8_t dummy;                                      /*!< Event not used currently */
} esp_ble_mesh_df_server_state_change_t;

typedef enum {
    ESP_BLE_MESH_DF_TABLE_ACT_EMPTY,
    ESP_BLE_MESH_DF_TABLE_ADD,
    ESP_BLE_MESH_DF_TABLE_REMOVE,
    ESP_BLE_MESH_DF_TABLE_ENTRY_CHANGE,
    ESP_BLE_MESH_DF_TABLE_ACT_MAX_LIMIT,
} esp_ble_mesh_df_table_action_t;

/** Parameters of directed forwarding table entry change */
typedef struct {
    esp_ble_mesh_df_table_action_t action;              /*!< Action of directed forwarding table */
    /** Union of directed forwarding table information */
    union {
        /** Structure of directed forwarding table add and remove */
        struct {
            esp_ble_mesh_uar_t path_origin;             /*!< Primary element address of the Path Origin */
            esp_ble_mesh_uar_t path_target;             /*!< Primary element address of the Path Target */

            esp_ble_mesh_uar_t *dep_origin_data;        /*!< List of the primary element addresses of the dependent nodes of the Path Origin */
            uint32_t dep_origin_num;                    /*!< Number of entries in the Dependent_Origin_List field of the message */

            esp_ble_mesh_uar_t *dep_target_data;        /*!< List of the primary element addresses of the dependent nodes of the Path Target */
            uint32_t dep_target_num;                    /*!< Number of entries in the Dependent_Target_List field of the message */

            uint8_t fixed_path:1,                       /*!< Indicates whether the table entry is a fixed path entry or a non-fixed path entry */
                    bw_path_validate:1,                 /*!< Indicates whether or not the backward path has been validated */
                    path_not_ready:1;                   /*!< Flag indicating whether or not the path is ready for use */

            uint8_t  forward_number;                    /*!< Forwarding number of the Path Origin; If the entry is associated with a fixed path, the value is 0 */

            uint8_t  lane_counter;                      /*!< Number of lanes discovered; if the entry is associated with a fixed path, the value is 1.*/
        } df_table_entry_add_remove;                    /*!< Structure of directed forwarding table add and remove */
        /** Structure of directed forwarding table entry change */
        struct {
            uint8_t dummy;                              /*!< Event not used currently */
        } df_table_entry_change;                        /*!< Directed forwarding table entry change */
    } df_table_info;                                    /*!< Directed forwarding table information */
} esp_ble_mesh_df_server_table_change_t;                /*!< Structure of directed forwarding table entry change */

/**
 * @brief Directed Forwarding Configuration Server model callback value union
 */
typedef union {
    esp_ble_mesh_df_server_state_change_t state_change; /*!< For ESP_BLE_MESH_DF_SERVER_STATE_CHANGE_EVT */
    esp_ble_mesh_df_server_table_change_t table_change; /*!< For ESP_BLE_MESH_DF_SERVER_TABLE_CHANGE_EVT */
} esp_ble_mesh_df_server_cb_value_t;

/** Directed Forwarding Configuration Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                       /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                         /*!< Context of the received message */
    esp_ble_mesh_df_server_cb_value_t value;            /*!< Value of the received configuration messages */
} esp_ble_mesh_df_server_cb_param_t;

/** This enum value is the event of Directed Forwarding Configuration Server model */
typedef enum {
    ESP_BLE_MESH_DF_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_DF_SERVER_TABLE_CHANGE_EVT,
    ESP_BLE_MESH_DF_SERVER_EVT_MAX,
} esp_ble_mesh_df_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Directed Forwarding Configuration client and server model functions.
 */

/**
 * @brief   Directed Forwarding Configuration Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_df_client_cb_t)(esp_ble_mesh_df_client_cb_event_t event,
                                             esp_ble_mesh_df_client_cb_param_t *param);

/**
 * @brief   Directed Forwarding Configuration Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_df_server_cb_t)(esp_ble_mesh_df_server_cb_event_t event,
                                             esp_ble_mesh_df_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Directed Forwarding Configuration Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_df_client_callback(esp_ble_mesh_df_client_cb_t callback);

/**
 * @brief       Register BLE Mesh Directed Forwarding Configuration Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_df_server_callback(esp_ble_mesh_df_server_cb_t callback);

/**
 * @brief       Get the value of Directed Forwarding Configuration Server model state with the corresponding get message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   get:    Pointer to a union, each kind of opcode corresponds to one structure inside.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_df_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                           esp_ble_mesh_df_client_get_t *get);

/**
 * @brief       Set the value of Directed Forwarding Configuration Server model state with the corresponding set message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   set:    Pointer to a union, each kind of opcode corresponds to one structure inside.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_df_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                           esp_ble_mesh_df_client_set_t *set);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_DF_MODEL_API_H_ */
