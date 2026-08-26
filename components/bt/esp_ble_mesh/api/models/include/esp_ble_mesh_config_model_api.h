/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_CONFIG_MODEL_API_H_
#define _ESP_BLE_MESH_CONFIG_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @def    ESP_BLE_MESH_MODEL_CFG_SRV
 *
 *  @brief  Define a new Config Server Model.
 *
 *  @note   The Config Server Model can only be included by a Primary Element.
 *
 *  @param  srv_data Pointer to a unique Config Server Model user_data.
 *
 *  @return New Config Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_CFG_SRV(srv_data)                              \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_CONFIG_SRV,          \
                  NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_CFG_CLI
 *
 *  @brief  Define a new Config Client Model.
 *
 *  @note   The Config Client Model can only be included by a Primary Element.
 *
 *  @param  cli_data Pointer to a unique struct esp_ble_mesh_client_t.
 *
 *  @return New Config Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_CFG_CLI(cli_data)                              \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_CONFIG_CLI,          \
                  NULL, NULL, cli_data)

/** Configuration Server Model context */
typedef struct {
    esp_ble_mesh_model_t *model;    /*!< Pointer to Configuration Server Model */

    uint8_t net_transmit;           /*!< Network Transmit state */
    uint8_t relay;                  /*!< Relay Mode state */
    uint8_t relay_retransmit;       /*!< Relay Retransmit state */
    uint8_t beacon;                 /*!< Secure Network Beacon state */
    uint8_t gatt_proxy;             /*!< GATT Proxy state */
    uint8_t friend_state;           /*!< Friend state */
    uint8_t default_ttl;            /*!< Default TTL */

    /** Heartbeat Publication */
    struct {
        struct k_delayed_work timer;    /*!< Heartbeat Publication timer */

        uint16_t dst;                   /*!< Destination address for Heartbeat messages */
        uint16_t count;                 /*!< Number of Heartbeat messages to be sent */
        uint8_t  period;                /*!< Period for sending Heartbeat messages */
        uint8_t  ttl;                   /*!< TTL to be used when sending Heartbeat messages */
        uint16_t feature;               /*!< Bit field indicating features that trigger Heartbeat messages when changed */
        uint16_t net_idx;               /*!< NetKey Index used by Heartbeat Publication */
    } heartbeat_pub;

    /** Heartbeat Subscription */
    struct {
        int64_t  expiry;                /*!< Timestamp when Heartbeat subscription period is expired */

        uint16_t src;                   /*!< Source address for Heartbeat messages */
        uint16_t dst;                   /*!< Destination address for Heartbeat messages */
        uint16_t count;                 /*!< Number of Heartbeat messages received */
        uint8_t  min_hops;              /*!< Minimum hops when receiving Heartbeat messages */
        uint8_t  max_hops;              /*!< Maximum hops when receiving Heartbeat messages */

        /** Optional heartbeat subscription tracking function */
        esp_ble_mesh_cb_t heartbeat_recv_cb;
    } heartbeat_sub;
} esp_ble_mesh_cfg_srv_t;

/** Parameters of Config Composition Data Get. */
typedef struct {
    uint8_t page;                   /*!< Page number of the Composition Data. */
} esp_ble_mesh_cfg_composition_data_get_t;

/** Parameters of Config Model Publication Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_pub_get_t;

/** Parameters of Config SIG Model Subscription Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
} esp_ble_mesh_cfg_sig_model_sub_get_t;

/** Parameters of Config Vendor Model Subscription Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_vnd_model_sub_get_t;

/** Parameters of Config AppKey Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} esp_ble_mesh_cfg_app_key_get_t;

/** Parameters of Config Node Identity Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} esp_ble_mesh_cfg_node_identity_get_t;

/** Parameters of Config SIG Model App Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
} esp_ble_mesh_cfg_sig_model_app_get_t;

/** Parameters of Config Vendor Model App Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_vnd_model_app_get_t;

/** Parameters of Config Key Refresh Phase Get. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} esp_ble_mesh_cfg_kr_phase_get_t;

/** Parameters of Config Low Power Node PollTimeout Get. */
typedef struct {
    uint16_t lpn_addr;              /*!< The unicast address of the Low Power node */
} esp_ble_mesh_cfg_lpn_polltimeout_get_t;

/** Parameters of Config Beacon Set. */
typedef struct {
    uint8_t beacon;                 /*!< New Secure Network Beacon state */
} esp_ble_mesh_cfg_beacon_set_t;

/** Parameters of Config Default TTL Set. */
typedef struct {
    uint8_t ttl;                    /*!< The default TTL state value */
} esp_ble_mesh_cfg_default_ttl_set_t;

/** Parameters of Config Friend Set. */
typedef struct {
    uint8_t friend_state;           /*!< The friend state value */
} esp_ble_mesh_cfg_friend_set_t;

/** Parameters of Config GATT Proxy Set. */
typedef struct {
    uint8_t gatt_proxy;             /*!< The GATT Proxy state value */
} esp_ble_mesh_cfg_gatt_proxy_set_t;

/** Parameters of Config Relay Set. */
typedef struct {
    uint8_t relay;                  /*!< The relay value */
    uint8_t relay_retransmit;       /*!< The relay retransmit value */
} esp_ble_mesh_cfg_relay_set_t;

/** Parameters of Config NetKey Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  net_key[16];           /*!< The network key value */
} esp_ble_mesh_cfg_net_key_add_t;

/** Parameters of Config AppKey Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
    uint8_t  app_key[16];           /*!< The app key value */
} esp_ble_mesh_cfg_app_key_add_t;

/** Parameters of Config Model App Bind. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_app_idx;         /*!< Index of the app key to bind with the model */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_app_bind_t;

/** Parameters of Config Model Publication Set. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t publish_addr;          /*!< Value of the publish address */
    uint16_t publish_app_idx;       /*!< Index of the application key */
    bool     cred_flag;             /*!< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;           /*!< Default TTL value for the publishing messages */
    uint8_t  publish_period;        /*!< Period for periodic status publishing */
    uint8_t  publish_retransmit;    /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_pub_set_t;

/** Parameters of Config Model Subscription Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_add_t;

/** Parameters of Config Model Subscription Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_delete_t;

/** Parameters of Config Model Subscription Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_overwrite_t;

/** Parameters of Config Model Subscription Virtual Address Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_add_t;

/** Parameters of Config Model Subscription Virtual Address Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_delete_t;

/** Parameters of Config Model Subscription Virtual Address Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_overwrite_t;

/** Parameters of Config Model Publication Virtual Address Set. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< Value of the Label UUID publish address */
    uint16_t publish_app_idx;       /*!< Index of the application key */
    bool     cred_flag;             /*!< Value of the Friendship Credential Flag */
    uint8_t  publish_ttl;           /*!< Default TTL value for the publishing messages */
    uint8_t  publish_period;        /*!< Period for periodic status publishing */
    uint8_t  publish_retransmit;    /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_pub_va_set_t;

/** Parameters of Config Model Subscription Delete All. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_delete_all_t;

/** Parameters of Config NetKey Update. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  net_key[16];           /*!< The network key value */
} esp_ble_mesh_cfg_net_key_update_t;

/** Parameters of Config NetKey Delete. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
} esp_ble_mesh_cfg_net_key_delete_t;

/** Parameters of Config AppKey Update. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
    uint8_t  app_key[16];           /*!< The app key value */
} esp_ble_mesh_cfg_app_key_update_t;

/** Parameters of Config AppKey Delete. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
} esp_ble_mesh_cfg_app_key_delete_t;

/** Parameters of Config Node Identity Set. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  identity;              /*!< New Node Identity state */
} esp_ble_mesh_cfg_node_identity_set_t;

/** Parameters of Config Model App Unbind. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_app_idx;         /*!< Index of the app key to bind with the model */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_app_unbind_t;

/** Parameters of Config Key Refresh Phase Set. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  transition;            /*!< New Key Refresh Phase Transition */
} esp_ble_mesh_cfg_kr_phase_set_t;

/** Parameters of Config Network Transmit Set. */
typedef struct {
    uint8_t net_transmit;           /*!< Network Transmit State */
} esp_ble_mesh_cfg_net_transmit_set_t;

/** Parameters of Config Model Heartbeat Publication Set. */
typedef struct {
    uint16_t dst;                   /*!< Destination address for Heartbeat messages */
    uint8_t  count;                 /*!< Number of Heartbeat messages to be sent */
    uint8_t  period;                /*!< Period for sending Heartbeat messages */
    uint8_t  ttl;                   /*!< TTL to be used when sending Heartbeat messages */
    uint16_t feature;               /*!< Bit field indicating features that trigger Heartbeat messages when changed */
    uint16_t net_idx;               /*!< NetKey Index */
} esp_ble_mesh_cfg_heartbeat_pub_set_t;

/** Parameters of Config Model Heartbeat Subscription Set. */
typedef struct {
    uint16_t src;                   /*!< Source address for Heartbeat messages */
    uint16_t dst;                   /*!< Destination address for Heartbeat messages */
    uint8_t  period;                /*!< Period for receiving Heartbeat messages */
} esp_ble_mesh_cfg_heartbeat_sub_set_t;

/**
 * @brief For ESP_BLE_MESH_MODEL_OP_BEACON_GET
 *            ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET
 *            ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET
 *            ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET
 *            ESP_BLE_MESH_MODEL_OP_RELAY_GET
 *            ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET
 *            ESP_BLE_MESH_MODEL_OP_FRIEND_GET
 *            ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET
 *            ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET
 * the get_state parameter in the esp_ble_mesh_config_client_get_state function should not be set to NULL.
 */
typedef union {
    esp_ble_mesh_cfg_model_pub_get_t         model_pub_get;     /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET. */
    esp_ble_mesh_cfg_composition_data_get_t  comp_data_get;     /*!< For ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET. */
    esp_ble_mesh_cfg_sig_model_sub_get_t     sig_model_sub_get; /*!< For ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET */
    esp_ble_mesh_cfg_vnd_model_sub_get_t     vnd_model_sub_get; /*!< For ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET */
    esp_ble_mesh_cfg_app_key_get_t           app_key_get;       /*!< For ESP_BLE_MESH_MODEL_OP_APP_KEY_GET. */
    esp_ble_mesh_cfg_node_identity_get_t     node_identity_get; /*!< For ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET. */
    esp_ble_mesh_cfg_sig_model_app_get_t     sig_model_app_get; /*!< For ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET */
    esp_ble_mesh_cfg_vnd_model_app_get_t     vnd_model_app_get; /*!< For ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET */
    esp_ble_mesh_cfg_kr_phase_get_t          kr_phase_get;      /*!< For ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET */
    esp_ble_mesh_cfg_lpn_polltimeout_get_t   lpn_pollto_get;    /*!< For ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET */
} esp_ble_mesh_cfg_client_get_state_t;

/**
 * @brief For ESP_BLE_MESH_MODEL_OP_BEACON_SET
 *            ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET
 *            ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET
 *            ESP_BLE_MESH_MODEL_OP_RELAY_SET
 *            ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE
 *            ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE
 *            ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD
 *            ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD
 *            ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND
 *            ESP_BLE_MESH_MODEL_OP_NODE_RESET
 *            ESP_BLE_MESH_MODEL_OP_FRIEND_SET
 *            ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET
 *            ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET
 * the set_state parameter in the esp_ble_mesh_config_client_set_state function should not be set to NULL.
 */
typedef union {
    esp_ble_mesh_cfg_beacon_set_t             beacon_set;             /*!< For ESP_BLE_MESH_MODEL_OP_BEACON_SET */
    esp_ble_mesh_cfg_default_ttl_set_t        default_ttl_set;        /*!< For ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET */
    esp_ble_mesh_cfg_friend_set_t             friend_set;             /*!< For ESP_BLE_MESH_MODEL_OP_FRIEND_SET */
    esp_ble_mesh_cfg_gatt_proxy_set_t         gatt_proxy_set;         /*!< For ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET */
    esp_ble_mesh_cfg_relay_set_t              relay_set;              /*!< For ESP_BLE_MESH_MODEL_OP_RELAY_SET */
    esp_ble_mesh_cfg_net_key_add_t            net_key_add;            /*!< For ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD */
    esp_ble_mesh_cfg_app_key_add_t            app_key_add;            /*!< For ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD */
    esp_ble_mesh_cfg_model_app_bind_t         model_app_bind;         /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND */
    esp_ble_mesh_cfg_model_pub_set_t          model_pub_set;          /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET */
    esp_ble_mesh_cfg_model_sub_add_t          model_sub_add;          /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD */
    esp_ble_mesh_cfg_model_sub_delete_t       model_sub_delete;       /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE */
    esp_ble_mesh_cfg_model_sub_overwrite_t    model_sub_overwrite;    /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE */
    esp_ble_mesh_cfg_model_sub_va_add_t       model_sub_va_add;       /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD */
    esp_ble_mesh_cfg_model_sub_va_delete_t    model_sub_va_delete;    /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE */
    esp_ble_mesh_cfg_model_sub_va_overwrite_t model_sub_va_overwrite; /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE */
    esp_ble_mesh_cfg_heartbeat_pub_set_t      heartbeat_pub_set;      /*!< For ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET */
    esp_ble_mesh_cfg_heartbeat_sub_set_t      heartbeat_sub_set;      /*!< For ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET */
    esp_ble_mesh_cfg_model_pub_va_set_t       model_pub_va_set;       /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET */
    esp_ble_mesh_cfg_model_sub_delete_all_t   model_sub_delete_all;   /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL */
    esp_ble_mesh_cfg_net_key_update_t         net_key_update;         /*!< For ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE */
    esp_ble_mesh_cfg_net_key_delete_t         net_key_delete;         /*!< For ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE */
    esp_ble_mesh_cfg_app_key_update_t         app_key_update;         /*!< For ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE */
    esp_ble_mesh_cfg_app_key_delete_t         app_key_delete;         /*!< For ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE */
    esp_ble_mesh_cfg_node_identity_set_t      node_identity_set;      /*!< For ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET */
    esp_ble_mesh_cfg_model_app_unbind_t       model_app_unbind;       /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND */
    esp_ble_mesh_cfg_kr_phase_set_t           kr_phase_set;           /*!< For ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET */
    esp_ble_mesh_cfg_net_transmit_set_t       net_transmit_set;       /*!< For ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET */
} esp_ble_mesh_cfg_client_set_state_t;

/** Parameter of Config Beacon Status */
typedef struct {
    uint8_t beacon;                     /*!< Secure Network Beacon state value */
} esp_ble_mesh_cfg_beacon_status_cb_t;

/** Parameters of Config Composition Data Status */
typedef struct {
    uint8_t page;                              /*!< Page number of the Composition Data */
    struct net_buf_simple *composition_data;   /*!< Pointer to Composition Data for the identified page */
} esp_ble_mesh_cfg_comp_data_status_cb_t;

/** Parameter of Config Default TTL Status */
typedef struct {
    uint8_t default_ttl;                /*!< Default TTL state value */
} esp_ble_mesh_cfg_default_ttl_status_cb_t;

/** Parameter of Config GATT Proxy Status */
typedef struct {
    uint8_t gatt_proxy;                 /*!< GATT Proxy state value */
} esp_ble_mesh_cfg_gatt_proxy_status_cb_t;

/** Parameters of Config Relay Status */
typedef struct {
    uint8_t relay;                      /*!< Relay state value */
    uint8_t retransmit;                 /*!< Relay retransmit value(number of retransmissions and number of 10-millisecond steps between retransmissions) */
} esp_ble_mesh_cfg_relay_status_cb_t;

/** Parameters of Config Model Publication Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t publish_addr;              /*!< Value of the publish address */
    uint16_t app_idx;                   /*!< Index of the application key */
    bool     cred_flag;                 /*!< Value of the Friendship Credential Flag */
    uint8_t  ttl;                       /*!< Default TTL value for the outgoing messages */
    uint8_t  period;                    /*!< Period for periodic status publishing */
    uint8_t  transmit;                  /*!< Number of retransmissions and number of 50-millisecond steps between retransmissions */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} esp_ble_mesh_cfg_model_pub_status_cb_t;

/** Parameters of Config Model Subscription Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t sub_addr;                  /*!< Value of the address */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} esp_ble_mesh_cfg_model_sub_status_cb_t;

/** Parameters of Config NetKey Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} esp_ble_mesh_cfg_net_key_status_cb_t;

/** Parameters of Config AppKey Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint16_t app_idx;                   /*!< Index of the application key */
} esp_ble_mesh_cfg_app_key_status_cb_t;

/** Parameters of Config Model App Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t app_idx;                   /*!< Index of the application key */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} esp_ble_mesh_cfg_mod_app_status_cb_t;

/** Parameter of Config Friend Status */
typedef struct {
    uint8_t friend_state;               /*!< Friend state value */
} esp_ble_mesh_cfg_friend_status_cb_t;

/** Parameters of Config Heartbeat Publication Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages remaining to be sent */
    uint8_t  period;                    /*!< Period for sending Heartbeat messages */
    uint8_t  ttl;                       /*!< TTL to be used when sending Heartbeat messages */
    uint16_t features;                  /*!< Features that trigger Heartbeat messages when changed */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} esp_ble_mesh_cfg_hb_pub_status_cb_t;

/** Parameters of Config Heartbeat Subscription Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t src;                       /*!< Source address for Heartbeat messages */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  period;                    /*!< Remaining Period for processing Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages received */
    uint8_t  min_hops;                  /*!< Minimum hops when receiving Heartbeat messages */
    uint8_t  max_hops;                  /*!< Maximum hops when receiving Heartbeat messages */
} esp_ble_mesh_cfg_hb_sub_status_cb_t;

/** Parameters of Config Network Transmit Status */
typedef struct {
    uint8_t net_trans_count: 3;         /*!< Number of transmissions for each Network PDU originating from the node */
    uint8_t net_trans_step : 5;         /*!< Maximum hops when receiving Heartbeat messages */
} esp_ble_mesh_cfg_net_trans_status_cb_t;

/** Parameters of Config SIG/Vendor Subscription List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
    struct net_buf_simple *sub_addr;    /*!< A block of all addresses from the Subscription List */
} esp_ble_mesh_cfg_model_sub_list_cb_t;

/** Parameter of Config NetKey List */
typedef struct {
    struct net_buf_simple *net_idx;     /*!< A list of NetKey Indexes known to the node */
} esp_ble_mesh_cfg_net_key_list_cb_t;

/** Parameters of Config AppKey List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< NetKey Index of the NetKey that the AppKeys are bound to */
    struct net_buf_simple *app_idx;     /*!< A list of AppKey indexes that are bound to the NetKey identified by NetKeyIndex */
} esp_ble_mesh_cfg_app_key_list_cb_t;

/** Parameters of Config Node Identity Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint8_t  identity;                  /*!< Node Identity state */
} esp_ble_mesh_cfg_node_id_status_cb_t;

/** Parameters of Config SIG/Vendor Model App List */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
    struct net_buf_simple *app_idx;     /*!< All AppKey indexes bound to the Model */
} esp_ble_mesh_cfg_model_app_list_cb_t;

/** Parameters of Config Key Refresh Phase Status */
typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint8_t  phase;                     /*!< Key Refresh Phase state */
} esp_ble_mesh_cfg_kr_phase_status_cb_t;

/** Parameters of Config Low Power Node PollTimeout Status */
typedef struct {
    uint16_t lpn_addr;                  /*!< The unicast address of the Low Power node */
    int32_t  poll_timeout;              /*!< The current value of the PollTimeout timer of the Low Power node */
} esp_ble_mesh_cfg_lpn_pollto_status_cb_t;

/**
 * @brief Configuration Client Model received message union
 */
typedef union {
    esp_ble_mesh_cfg_beacon_status_cb_t      beacon_status;         /*!< The beacon status value */
    esp_ble_mesh_cfg_comp_data_status_cb_t   comp_data_status;      /*!< The composition data status value */
    esp_ble_mesh_cfg_default_ttl_status_cb_t default_ttl_status;    /*!< The default_ttl status value */
    esp_ble_mesh_cfg_gatt_proxy_status_cb_t  gatt_proxy_status;     /*!< The gatt_proxy status value */
    esp_ble_mesh_cfg_relay_status_cb_t       relay_status;          /*!< The relay status value */
    esp_ble_mesh_cfg_model_pub_status_cb_t   model_pub_status;      /*!< The model publication status value */
    esp_ble_mesh_cfg_model_sub_status_cb_t   model_sub_status;      /*!< The model subscription status value */
    esp_ble_mesh_cfg_net_key_status_cb_t     netkey_status;         /*!< The netkey status value */
    esp_ble_mesh_cfg_app_key_status_cb_t     appkey_status;         /*!< The appkey status value */
    esp_ble_mesh_cfg_mod_app_status_cb_t     model_app_status;      /*!< The model app status value */
    esp_ble_mesh_cfg_friend_status_cb_t      friend_status;         /*!< The friend status value */
    esp_ble_mesh_cfg_hb_pub_status_cb_t      heartbeat_pub_status;  /*!< The heartbeat publication status value */
    esp_ble_mesh_cfg_hb_sub_status_cb_t      heartbeat_sub_status;  /*!< The heartbeat subscription status value */
    esp_ble_mesh_cfg_net_trans_status_cb_t   net_transmit_status;   /*!< The network transmit status value */
    esp_ble_mesh_cfg_model_sub_list_cb_t     model_sub_list;        /*!< The model subscription list value */
    esp_ble_mesh_cfg_net_key_list_cb_t       netkey_list;           /*!< The network key index list value */
    esp_ble_mesh_cfg_app_key_list_cb_t       appkey_list;           /*!< The application key index list value */
    esp_ble_mesh_cfg_node_id_status_cb_t     node_identity_status;  /*!< The node identity status value */
    esp_ble_mesh_cfg_model_app_list_cb_t     model_app_list;        /*!< The model application key index list value */
    esp_ble_mesh_cfg_kr_phase_status_cb_t    kr_phase_status;       /*!< The key refresh phase status value */
    esp_ble_mesh_cfg_lpn_pollto_status_cb_t  lpn_timeout_status;    /*!< The low power node poll timeout status value */
} esp_ble_mesh_cfg_client_common_cb_param_t;

/** Configuration Client Model callback parameters */
typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t       *params;       /*!< The client common parameters */
    esp_ble_mesh_cfg_client_common_cb_param_t status_cb;    /*!< The config status message callback values */
} esp_ble_mesh_cfg_client_cb_param_t;

/** This enum value is the event of Configuration Client Model */
typedef enum {
    ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_CFG_CLIENT_EVT_MAX,
} esp_ble_mesh_cfg_client_cb_event_t;

/**
 * @brief Configuration Server model related context.
 */

/** Parameters of Config Model Publication Set */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t pub_addr;          /*!< Publish Address */
    uint16_t app_idx;           /*!< AppKey Index */
    bool     cred_flag;         /*!< Friendship Credential Flag */
    uint8_t  pub_ttl;           /*!< Publish TTL */
    uint8_t  pub_period;        /*!< Publish Period */
    uint8_t  pub_retransmit;    /*!< Publish Retransmit */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_mod_pub_set_t;

/** Parameters of Config Model Publication Virtual Address Set */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint8_t  label_uuid[16];    /*!< Label UUID */
    uint16_t app_idx;           /*!< AppKey Index */
    bool     cred_flag;         /*!< Friendship Credential Flag */
    uint8_t  pub_ttl;           /*!< Publish TTL */
    uint8_t  pub_period;        /*!< Publish Period */
    uint8_t  pub_retransmit;    /*!< Publish Retransmit */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_mod_pub_va_set_t;

/** Parameters of Config Model Subscription Add */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t sub_addr;          /*!< Subscription Address */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_model_sub_add_t;

/** Parameters of Config Model Subscription Delete */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t sub_addr;          /*!< Subscription Address */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_model_sub_delete_t;

/** Parameters of Config NetKey Add */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  net_key[16];       /*!< NetKey */
} esp_ble_mesh_state_change_cfg_netkey_add_t;

/** Parameters of Config NetKey Update */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  net_key[16];       /*!< NetKey */
} esp_ble_mesh_state_change_cfg_netkey_update_t;

/** Parameter of Config NetKey Delete */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
} esp_ble_mesh_state_change_cfg_netkey_delete_t;

/** Parameters of Config AppKey Add */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
    uint8_t  app_key[16];       /*!< AppKey */
} esp_ble_mesh_state_change_cfg_appkey_add_t;

/** Parameters of Config AppKey Update */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
    uint8_t  app_key[16];       /*!< AppKey */
} esp_ble_mesh_state_change_cfg_appkey_update_t;

/** Parameters of Config AppKey Delete */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint16_t app_idx;           /*!< AppKey Index */
} esp_ble_mesh_state_change_cfg_appkey_delete_t;

/** Parameters of Config Model App Bind */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t app_idx;           /*!< AppKey Index */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_model_app_bind_t;

/** Parameters of Config Model App Unbind */
typedef struct {
    uint16_t element_addr;      /*!< Element Address */
    uint16_t app_idx;           /*!< AppKey Index */
    uint16_t company_id;        /*!< Company ID */
    uint16_t model_id;          /*!< Model ID */
} esp_ble_mesh_state_change_cfg_model_app_unbind_t;

/** Parameters of Config Key Refresh Phase Set */
typedef struct {
    uint16_t net_idx;           /*!< NetKey Index */
    uint8_t  kr_phase;          /*!< New Key Refresh Phase Transition */
} esp_ble_mesh_state_change_cfg_kr_phase_set_t;

/**
 * @brief Configuration Server model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_cfg_mod_pub_set_t         mod_pub_set;        /*!< Config Model Publication Set */
    esp_ble_mesh_state_change_cfg_mod_pub_va_set_t      mod_pub_va_set;     /*!< Config Model Publication Virtual Address Set */
    esp_ble_mesh_state_change_cfg_model_sub_add_t       mod_sub_add;        /*!< Config Model Subscription Add */
    esp_ble_mesh_state_change_cfg_model_sub_delete_t    mod_sub_delete;     /*!< Config Model Subscription Delete */
    esp_ble_mesh_state_change_cfg_netkey_add_t          netkey_add;         /*!< Config NetKey Add */
    esp_ble_mesh_state_change_cfg_netkey_update_t       netkey_update;      /*!< Config NetKey Update */
    esp_ble_mesh_state_change_cfg_netkey_delete_t       netkey_delete;      /*!< Config NetKey Delete */
    esp_ble_mesh_state_change_cfg_appkey_add_t          appkey_add;         /*!< Config AppKey Add */
    esp_ble_mesh_state_change_cfg_appkey_update_t       appkey_update;      /*!< Config AppKey Update */
    esp_ble_mesh_state_change_cfg_appkey_delete_t       appkey_delete;      /*!< Config AppKey Delete */
    esp_ble_mesh_state_change_cfg_model_app_bind_t      mod_app_bind;       /*!< Config Model App Bind */
    esp_ble_mesh_state_change_cfg_model_app_unbind_t    mod_app_unbind;     /*!< Config Model App Unbind */
    esp_ble_mesh_state_change_cfg_kr_phase_set_t        kr_phase_set;       /*!< Config Key Refresh Phase Set */
} esp_ble_mesh_cfg_server_state_change_t;

/**
 * @brief Configuration Server model callback value union
 */
typedef union {
    esp_ble_mesh_cfg_server_state_change_t state_change;  /*!< ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_cfg_server_cb_value_t;

/** Configuration Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;   /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received message */
    esp_ble_mesh_cfg_server_cb_value_t value;   /*!< Value of the received configuration messages */
} esp_ble_mesh_cfg_server_cb_param_t;

/** This enum value is the event of Configuration Server model */
typedef enum {
    ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_CFG_SERVER_EVT_MAX,
} esp_ble_mesh_cfg_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Config Client and Server Model functions.
 */

/**
 * @brief   Configuration Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_cfg_client_cb_t)(esp_ble_mesh_cfg_client_cb_event_t event,
                                              esp_ble_mesh_cfg_client_cb_param_t *param);

/**
 * @brief   Configuration Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_cfg_server_cb_t)(esp_ble_mesh_cfg_server_cb_event_t event,
                                              esp_ble_mesh_cfg_server_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Config Client Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_config_client_callback(esp_ble_mesh_cfg_client_cb_t callback);

/**
 * @brief         Register BLE Mesh Config Server Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_config_server_callback(esp_ble_mesh_cfg_server_cb_t callback);

/**
 * @brief         Get the value of Config Server Model states using the Config Client Model get messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to esp_ble_mesh_opcode_config_client_get_t in esp_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     get_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_config_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_cfg_client_get_state_t *get_state);

/**
 * @brief         Set the value of the Configuration Server Model states using the Config Client Model set messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to esp_ble_mesh_opcode_config_client_set_t in esp_ble_mesh_defs.h
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     set_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_config_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_cfg_client_set_state_t *set_state);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_CONFIG_MODEL_API_H_ */
