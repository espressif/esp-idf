// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_BLE_MESH_CONFIG_MODEL_API_H_
#define _ESP_BLE_MESH_CONFIG_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

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

typedef struct esp_ble_mesh_cfg_srv {
    esp_ble_mesh_model_t *model;

    uint8_t net_transmit;         /*!< Network Transmit state */
    uint8_t relay;                /*!< Relay Mode state */
    uint8_t relay_retransmit;     /*!< Relay Retransmit state */
    uint8_t beacon;               /*!< Secure Network Beacon state */
    uint8_t gatt_proxy;           /*!< GATT Proxy state */
    uint8_t friend_state;         /*!< Friend state */
    uint8_t default_ttl;          /*!< Default TTL */

    /** Heartbeat Publication */
    struct {
        struct k_delayed_work timer;

        uint16_t dst;
        uint16_t count;
        uint8_t  period;
        uint8_t  ttl;
        uint16_t feature;
        uint16_t net_idx;
    } heartbeat_pub;

    /** Heartbeat Subscription */
    struct {
        int64_t  expiry;

        uint16_t src;
        uint16_t dst;
        uint16_t count;
        uint8_t  min_hops;
        uint8_t  max_hops;

        /** Optional subscription tracking function */
        void (*func)(uint8_t hops, uint16_t feature);
    } heartbeat_sub;
} esp_ble_mesh_cfg_srv_t;

/** Parameters of Composition Data Get. */
typedef struct {
    uint8_t page;                   /*!< Page number of the Composition Data. */
} esp_ble_mesh_composition_data_get_t;

/** Parameters of Model Publication Get. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_pub_get_t;

/** Parameters of Beacon Set. */
typedef struct {
    uint8_t beacon;
} esp_ble_mesh_cfg_beacon_set_t;

/** Parameters of Default TTL Set. */
typedef struct {
    uint8_t ttl;                    /*!< The default TTL state value */
} esp_ble_mesh_cfg_default_ttl_set_t;

/** Parameters of Friend Set. */
typedef struct {
    uint8_t friend_state;           /*!< The friend state value */
} esp_ble_mesh_cfg_friend_set_t;

/** Parameters of GATT Proxy Set. */
typedef struct {
    uint8_t gatt_proxy;             /*!< The GATT Proxy state value */
} esp_ble_mesh_cfg_gatt_proxy_set_t;

/** Parameters of Relay Set. */
typedef struct {
    uint8_t relay;                  /*!< The relay value */
    uint8_t relay_retransmit;       /*!< The relay retransmit value */
} esp_ble_mesh_cfg_relay_set_t;

/** Parameters of Network Key Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint8_t  net_key[16];           /*!< The network key value */
} esp_ble_mesh_cfg_net_key_add_t;

/** Parameters of Application Key Add. */
typedef struct {
    uint16_t net_idx;               /*!< The network key index */
    uint16_t app_idx;               /*!< The app key index */
    uint8_t  app_key[16];           /*!< The app key value */
} esp_ble_mesh_cfg_app_key_add_t;

/** Parameters of Model Application Key Bind. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t model_app_idx;         /*!< Index of the app key to bind with the model */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_app_bind_t;

/** Parameters of Model Publication Set. */
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

/** Parameters of Model Subscription Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_add_t;

/** Parameters of Model Subscription Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_delete_t;

/** Parameters of Model Subscription Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint16_t sub_addr;              /*!< The address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_overwrite_t;

/** Parameters of Model Subscription Virtual Address Add. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_add_t;

/** Parameters of Model Subscription Virtual Address Delete. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be removed from the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_delete_t;

/** Parameters of Model Subscription Virtual Address Overwrite. */
typedef struct {
    uint16_t element_addr;          /*!< The element address */
    uint8_t  label_uuid[16];        /*!< The Label UUID of the virtual address to be added to the Subscription List */
    uint16_t model_id;              /*!< The model id */
    uint16_t company_id;            /*!< The company id, if not a vendor model, shall set to 0xFFFF */
} esp_ble_mesh_cfg_model_sub_va_overwrite_t;

/** Parameters of Model Heartbeat Publication Set. */
typedef struct  {
    uint16_t dst;
    uint8_t  count;
    uint8_t  period;
    uint8_t  ttl;
    uint16_t feature;
    uint16_t net_idx;
} esp_ble_mesh_cfg_heartbeat_pub_set_t;

/** Parameters of Model Heartbeat Subscription Set. */
typedef struct {
    uint16_t src;
    uint16_t dst;
    uint8_t  period;
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
    esp_ble_mesh_cfg_model_pub_get_t    model_pub_get;       /*!< For ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET. */
    esp_ble_mesh_composition_data_get_t comp_data_get;       /*!< For ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET. */
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
} esp_ble_mesh_cfg_client_set_state_t;

typedef struct {
    uint8_t beacon;                     /*!< Secure Network Beacon state value */
} esp_ble_mesh_cfg_beacon_status_cb_t;

typedef struct {
    uint8_t page;                              /*!< Page number of the Composition Data */
    struct net_buf_simple *composition_data;   /*!< Pointer to Composition Data for the identified page */
} esp_ble_mesh_cfg_comp_data_status_cb_t;

typedef struct {
    uint8_t default_ttl;                /*!< Default TTL state value */
} esp_ble_mesh_cfg_default_ttl_status_cb_t;

typedef struct {
    uint8_t gatt_proxy;                 /*!< GATT Proxy state value */
} esp_ble_mesh_cfg_gatt_proxy_status_cb_t;

typedef struct {
    uint8_t relay;                      /*!< Relay state value */
    uint8_t retransmit;                 /*!< Relay retransmit value(number of retransmissions and number of 10-millisecond steps between retransmissions) */
} esp_ble_mesh_cfg_relay_status_cb_t;

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
} esp_ble_mesh_cfg_mod_pub_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t sub_addr;                  /*!< Value of the address */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} esp_ble_mesh_cfg_mod_sub_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} esp_ble_mesh_cfg_netkey_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t net_idx;                   /*!< Index of the NetKey */
    uint16_t app_idx;                   /*!< Index of the application key */
} esp_ble_mesh_cfg_appkey_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t element_addr;              /*!< Address of the element */
    uint16_t app_idx;                   /*!< Index of the application key */
    uint16_t company_id;                /*!< Company ID */
    uint16_t model_id;                  /*!< Model ID */
} esp_ble_mesh_cfg_mod_app_status_cb_t;

typedef struct {
    uint8_t friend_state;               /*!< Friend state value */
} esp_ble_mesh_cfg_friend_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages remaining to be sent */
    uint8_t  period;                    /*!< Period for sending Heartbeat messages */
    uint8_t  ttl;                       /*!< TTL to be used when sending Heartbeat messages */
    uint16_t features;                  /*!< Features that trigger Heartbeat messages when changed */
    uint16_t net_idx;                   /*!< Index of the NetKey */
} esp_ble_mesh_cfg_hb_pub_status_cb_t;

typedef struct {
    uint8_t  status;                    /*!< Status Code for the request message */
    uint16_t src;                       /*!< Source address for Heartbeat messages */
    uint16_t dst;                       /*!< Destination address for Heartbeat messages */
    uint8_t  period;                    /*!< Remaining Period for processing Heartbeat messages */
    uint8_t  count;                     /*!< Number of Heartbeat messages received */
    uint8_t  min_hops;                  /*!< Minimum hops when receiving Heartbeat messages */
    uint8_t  max_hops;                  /*!< Maximum hops when receiving Heartbeat messages */
} esp_ble_mesh_cfg_hb_sub_status_cb_t;

typedef union {
    esp_ble_mesh_cfg_beacon_status_cb_t      beacon_status;         /*!< The beacon status value */
    esp_ble_mesh_cfg_comp_data_status_cb_t   comp_data_status;      /*!< The composition data status value */
    esp_ble_mesh_cfg_default_ttl_status_cb_t default_ttl_status;    /*!< The default_ttl status value */
    esp_ble_mesh_cfg_gatt_proxy_status_cb_t  gatt_proxy_status;     /*!< The gatt_proxy status value */
    esp_ble_mesh_cfg_relay_status_cb_t       relay_status;          /*!< The relay status value */
    esp_ble_mesh_cfg_mod_pub_status_cb_t     model_pub_status;      /*!< The model publication status value */
    esp_ble_mesh_cfg_mod_sub_status_cb_t     model_sub_status;      /*!< The model subscription status value */
    esp_ble_mesh_cfg_netkey_status_cb_t      netkey_status;         /*!< The netkey status value */
    esp_ble_mesh_cfg_appkey_status_cb_t      appkey_status;         /*!< The appkey status value */
    esp_ble_mesh_cfg_mod_app_status_cb_t     model_app_status;      /*!< The model app status value */
    esp_ble_mesh_cfg_friend_status_cb_t      friend_status;         /*!< The friend status value */
    esp_ble_mesh_cfg_hb_pub_status_cb_t      heartbeat_pub_status;  /*!< The heartbeat publication status value */
    esp_ble_mesh_cfg_hb_sub_status_cb_t      heartbeat_sub_status;  /*!< The heartbeat subscription status value */
} esp_ble_mesh_cfg_client_common_cb_param_t;

typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t       *params;       /*!< The client common parameters */
    esp_ble_mesh_cfg_client_common_cb_param_t status_cb;    /*!< The config status message callback values */
} esp_ble_mesh_cfg_client_cb_param_t;

typedef enum {
    ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_CFG_CLIENT_EVT_MAX,
} esp_ble_mesh_cfg_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Config Client Model function.
 */

/** @brief: event, event code of Config Client Model events; param, parameters of Config Client Model events */
typedef void (* esp_mesh_cfg_client_cb_t)(esp_ble_mesh_cfg_client_cb_event_t event,
        esp_ble_mesh_cfg_client_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Config Client Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_config_client_callback(esp_mesh_cfg_client_cb_t callback);

/**
 * @brief         Get the value of Config Server Model states using the Config Client Model get messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to (@ref esp_ble_mesh_opcode_config_client_get_t).
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
 *                please refer to (@ref esp_ble_mesh_opcode_config_client_set_t).
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

#endif /** _ESP_BLE_MESH_CONFIG_MODEL_API_H_ */

