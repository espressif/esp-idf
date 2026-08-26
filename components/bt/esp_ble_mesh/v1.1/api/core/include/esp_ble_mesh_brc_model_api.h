/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_BRC_MODEL_API_H_
#define _ESP_BLE_MESH_BRC_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_GET             ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB1)
#define ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_SET             ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB2)
#define ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB3)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_ADD            ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB4)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_REMOVE         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB5)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_STATUS         ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB6)
#define ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET           ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB7)
#define ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_LIST          ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB8)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET            ESP_BLE_MESH_MODEL_OP_2(0x80, 0xB9)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_LIST           ESP_BLE_MESH_MODEL_OP_2(0x80, 0xBA)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_SIZE_GET       ESP_BLE_MESH_MODEL_OP_2(0x80, 0xBB)
#define ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_SIZE_STATUS    ESP_BLE_MESH_MODEL_OP_2(0x80, 0xBC)

/** @def    ESP_BLE_MESH_MODEL_BRC_SRV
 *
 *  @brief  Define a new Bridge Configuration Server model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  srv_data Pointer to a unique Bridge Configuration Server
 *          model user_data.
 *
 *  @return New Bridge Configuration Server model instance.
 */
#define ESP_BLE_MESH_MODEL_BRC_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_BRC_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_BRC_CLI
 *
 *  @brief  Define a new Bridge Configuration Client model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique Bridge Configuration Client
 *          model user_data.
 *
 *  @return New Bridge Configuration Client model instance.
 */
#define ESP_BLE_MESH_MODEL_BRC_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_BRC_CLI, \
                               NULL, NULL, cli_data)

/** Parameters of subnet bridge table */
typedef struct {
    uint8_t  bridge_direction;                  /*!< Allowed directions for the bridged traffic */
    uint8_t  bridge_net_idx[3];                 /*!< Two NetKey Indexes are packed into three octets */
    uint16_t bridge_addr_1;                     /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                     /*!< Address of the node in the second subnet */
} esp_ble_mesh_subnet_bridge_table_t;           /*!< Entries of subnet bridge table */

#if CONFIG_BLE_MESH_BRC_SRV
/** Bridge Configuration Server model context */
typedef struct {
    esp_ble_mesh_model_t  *model;               /*!< Pointer to Bridge Configuration Server model */

    uint8_t  subnet_bridge;                     /*!< Subnet Bridge state */
    uint16_t bridging_table_size;               /*!< Bridging Table Size */

    esp_ble_mesh_subnet_bridge_table_t bridge_table[CONFIG_BLE_MESH_MAX_BRIDGING_TABLE_ENTRY_COUNT];    /*!< Subnet Bridge table, used to storage bridging table entries */
} esp_ble_mesh_brc_srv_t;                       /*!< Bridge Configuration Server model context */
#endif

/** Parameters of Subnet Bridge Set */
typedef struct {
    uint8_t subnet_bridge;                      /*!< New Subnet Bridge state */
} esp_ble_mesh_subnet_bridge_set_t;             /*!< Parameters of Subnet Bridge Set */

/** Parameters of Bridging Table Add */
typedef struct {
    uint8_t  bridge_direction;                  /*!< Allowed directions for the bridged traffic */
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
    uint16_t bridge_addr_1;                     /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                     /*!< Address of the node in the second subnet */
} esp_ble_mesh_bridging_table_add_t;            /*!< Parameters of Bridging Table Add */

/** Parameters of Bridging Table Remove */
typedef struct {
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
    uint16_t bridge_addr_1;                     /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                     /*!< Address of the node in the second subnet */
} esp_ble_mesh_bridging_table_remove_t;         /*!< Parameters of Bridging Table Remove */

/** Parameters of Bridged Subnets Get */
typedef struct {
    uint16_t  bridge_filter:2,                  /*!< Filter to be applied when reporting the set of pairs of NetKey Indexes */
              bridge_net_idx:12;                /*!< NetKey Index of any of the subnets */
    uint8_t   bridge_start_idx;                 /*!< Start offset in units of Bridging Table state entries */
} esp_ble_mesh_bridged_subnets_get_t;           /*!< Parameters of Bridged Subnets Get */

/** Parameters of Bridging Table Get */
typedef struct {
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
    uint16_t bridge_start_idx;                  /*!< Start offset in units of Bridging Table state entries */
} esp_ble_mesh_bridging_table_get_t;            /*!< Parameters of Bridging Table Get */

/**
 * @brief Bridge Configuration Client model message union
 */
typedef union {
    esp_ble_mesh_bridged_subnets_get_t   bridged_subnets_get;   /*!< For ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_GET */
    esp_ble_mesh_bridging_table_get_t    bridging_table_get;    /*!< For ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_GET */
    esp_ble_mesh_subnet_bridge_set_t     subnet_bridge_set;     /*!< For ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_SET */
    esp_ble_mesh_bridging_table_add_t    bridging_table_add;    /*!< For ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_ADD */
    esp_ble_mesh_bridging_table_remove_t bridging_table_remove; /*!< For ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_REMOVE */
} esp_ble_mesh_brc_client_msg_t;                                /*!< Bridge Configuration Client model message union */

/** Parameters of Subnet Bridge Status */
typedef struct {
    uint8_t  subnet_bridge;                     /*!< Current Subnet Bridge state */
} esp_ble_mesh_subnet_bridge_status_t;          /*!< Parameters of Subnet Bridge Status */

/** Parameters of Bridging Table Status */
typedef struct {
    uint8_t  status;                            /*!< Status Code for the requesting message */
    uint8_t  bridge_direction;                  /*!< Allowed directions for the bridged traffic */
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
    uint16_t bridge_addr_1;                     /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                     /*!< Address of the node in the second subnet */
} esp_ble_mesh_bridging_table_status_t;         /*!< Parameters of Bridging Table Status */

/** Bridged_Subnets_List entry format */
typedef struct {
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
} esp_ble_mesh_bridge_net_idx_pair_entry_t;     /*!< Bridged_Subnets_List entry format */

/** Parameters of Bridged Subnets List */
typedef struct {
    uint16_t bridge_filter:2,                   /*!< Filter applied to the set of pairs of NetKey Indexes */
             bridge_net_idx:12;                 /*!< NetKey Index used for filtering or ignored */
    uint8_t  bridge_start_idx;                  /*!< Start offset in units of bridges */

    uint8_t  bridged_entry_list_size;           /*!< Num of pairs of NetKey Indexes */
    esp_ble_mesh_bridge_net_idx_pair_entry_t *net_idx_pair; /*!< Filtered set of N pairs of NetKey Indexes */
} esp_ble_mesh_bridged_subnets_list_t;          /*!< Parameters of Bridged Subnets List */

/** Bridged_Addresses_List entry format */
typedef struct {
    uint8_t  bridge_direction;                  /*!< Allowed directions for bridged traffic */
    uint16_t bridge_addr_1;                     /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                     /*!< Address of the node in the second subnet */
}esp_ble_mesh_bridged_addr_list_entry_t;        /*!< Bridged_Addresses_List entry format */

/** Parameters of Bridging Table List */
typedef struct {
    uint8_t  status;                            /*!< Status Code for the requesting message */
    uint16_t bridge_net_idx_1;                  /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                  /*!< NetKey Index of the second subnet */
    uint16_t bridge_start_idx;                  /*!< Start offset in units of Bridging Table state entries */
    uint16_t bridged_addr_list_size;            /*!< Num of pairs of entry */
    esp_ble_mesh_bridged_addr_list_entry_t *bridged_addr_list;  /*!< List of bridged addresses and allowed traffic directions */
} esp_ble_mesh_bridging_table_list_t;           /*!< Parameters of Bridging Table List */

/** Parameters of Bridging Table Size Status */
typedef struct {
    uint16_t bridging_table_size;               /*!< Bridging Table Size state */
} esp_ble_mesh_bridging_table_size_status_t;    /*!< Parameters of Bridging Table Size Status */

/**
 * @brief Bridge Configuration Client model received message union
 */
typedef union {
    esp_ble_mesh_subnet_bridge_status_t       subnet_bridge_status;         /*!< ESP_BLE_MESH_MODEL_OP_SUBNET_BRIDGE_STATUS */
    esp_ble_mesh_bridging_table_status_t      bridging_table_status;        /*!< ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_STATUS */
    esp_ble_mesh_bridged_subnets_list_t       bridged_subnets_list;         /*!< ESP_BLE_MESH_MODEL_OP_BRIDGED_SUBNETS_LIST */
    esp_ble_mesh_bridging_table_list_t        bridging_table_list;          /*!< ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_LIST */
    esp_ble_mesh_bridging_table_size_status_t bridging_table_size_status;   /*!< ESP_BLE_MESH_MODEL_OP_BRIDGING_TABLE_SIZE_STATUS */
} esp_ble_mesh_brc_client_recv_cb_t;/*!< */

/** Result of sending Bridge Configuration Client messages */
typedef struct {
    int err_code;                               /*!< Result of sending a message */
} esp_ble_mesh_brc_client_send_cb_t;            /*!< Result of sending Bridge Configuration Client messages */

/** Bridge Configuration Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params; /*!< Client common parameters, used by all events. */
    /** Union of Bridge Configuration Client callback */
    union {
        esp_ble_mesh_brc_client_send_cb_t send; /*!< Result of sending a message */
        esp_ble_mesh_brc_client_recv_cb_t recv; /*!< Parameters of received status message */
    };
} esp_ble_mesh_brc_client_cb_param_t;           /*!< Bridge Configuration Client model callback parameters */

/** This enum value is the event of Bridge Configuration Client model */
typedef enum {
    ESP_BLE_MESH_BRC_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_BRC_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_BRC_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_BRC_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_BRC_CLIENT_EVT_MAX,
} esp_ble_mesh_brc_client_cb_event_t;

/**
 * @brief Bridge Configuration Server model related context.
 */
/** Parameters of Bridging Table Add */
typedef struct {
    uint8_t  bridge_direction;                              /*!< Allowed directions for the bridged traffic */
    uint16_t bridge_net_idx_1;                              /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                              /*!< NetKey Index of the second subnet */
    uint16_t bridge_addr_1;                                 /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                                 /*!< Address of the node in the second subnet */
} esp_ble_mesh_state_change_bridging_table_add_t;           /*!< Parameters of Bridging Table Add */

/** Parameters of Bridging Table Remove */
typedef struct {
    uint16_t bridge_net_idx_1;                              /*!< NetKey Index of the first subnet */
    uint16_t bridge_net_idx_2;                              /*!< NetKey Index of the second subnet */
    uint16_t bridge_addr_1;                                 /*!< Address of the node in the first subnet */
    uint16_t bridge_addr_2;                                 /*!< Address of the node in the second subnet */
} esp_ble_mesh_state_change_bridging_table_remove_t;        /*!< Parameters of Bridging Table Remove */

/**
 * @brief Bridge Configuration Server model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_bridging_table_add_t    bridging_table_add;       /*!< Bridging Table Add */
    esp_ble_mesh_state_change_bridging_table_remove_t bridging_table_remove;    /*!< Bridging Table Remove*/
} esp_ble_mesh_brc_server_state_change_t;                                       /*!< Bridge Configuration Server model state change value union */

/**
 * @brief Bridge Configuration Server model callback value union
 */
typedef union {
    esp_ble_mesh_brc_server_state_change_t state_change;    /*!< For ESP_BLE_MESH_BRC_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_brc_server_cb_value_t;                       /*!< Bridge Configuration Server model callback value union */

/** Bridge Configuration Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                           /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                             /*!< Context of the received message */
    esp_ble_mesh_brc_server_cb_value_t value;               /*!< Value of the received configuration messages */
} esp_ble_mesh_brc_server_cb_param_t;                       /*!< Bridge Configuration Server model callback parameters */

/** This enum value is the event of Bridge Configuration Server model */
typedef enum {
    ESP_BLE_MESH_BRC_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_BRC_SERVER_EVT_MAX,
} esp_ble_mesh_brc_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Bridge Configuration client and server model functions.
 */

/**
 * @brief   Bridge Configuration Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_brc_client_cb_t)(esp_ble_mesh_brc_client_cb_event_t event,
                                              esp_ble_mesh_brc_client_cb_param_t *param);

/**
 * @brief   Bridge Configuration Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_brc_server_cb_t)(esp_ble_mesh_brc_server_cb_event_t event,
                                              esp_ble_mesh_brc_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Bridge Configuration Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_brc_client_callback(esp_ble_mesh_brc_client_cb_t callback);

/**
 * @brief       Register BLE Mesh Bridge Configuration Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_brc_server_callback(esp_ble_mesh_brc_server_cb_t callback);

/**
 * @brief       Get/Set the value of Bridge Configuration Server model state with the corresponding message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to Bridge Configuration Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_brc_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_brc_client_msg_t *msg);
#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_BRC_MODEL_API_H_ */
