/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_PRB_MODEL_API_H_
#define _ESP_BLE_MESH_PRB_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_PRIV_BEACON_GET               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x60)
#define ESP_BLE_MESH_MODEL_OP_PRIV_BEACON_SET               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x61)
#define ESP_BLE_MESH_MODEL_OP_PRIV_BEACON_STATUS            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x62)
#define ESP_BLE_MESH_MODEL_OP_PRIV_GATT_PROXY_GET           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x63)
#define ESP_BLE_MESH_MODEL_OP_PRIV_GATT_PROXY_SET           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x64)
#define ESP_BLE_MESH_MODEL_OP_PRIV_GATT_PROXY_STATUS        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x65)
#define ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_GET        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x66)
#define ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_SET        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x67)
#define ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_STATUS     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x68)

/** @def    ESP_BLE_MESH_MODEL_PRB_SRV
 *
 *  @brief  Define a new Private Beacon Server Model.
 *
 *  @note   The Private Beacon Server Model can only be included by a Primary Element.
 *
 *  @param  srv_data Pointer to a unique Private Beacon Server Model user_data.
 *
 *  @return New Private Beacon Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_PRB_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_PRB_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_PRB_CLI
 *
 *  @brief  Define a new Private Beacon Client Model.
 *
 *  @note   The Private Beacon Client Model can only be included by a Primary Element.
 *
 *  @param  cli_data Pointer to a unique struct esp_ble_mesh_client_t.
 *
 *  @return New Private Beacon Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_PRB_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_PRB_CLI, \
                               NULL, NULL, cli_data)

/** Private Beacon Server Model context */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to Private Beacon Server Model */

    uint8_t private_beacon;                             /*!< Value of Private Beacon state */
    uint8_t random_update_interval;                     /*!< Value of Random Update Interval Steps state */
    uint8_t private_gatt_proxy;                         /*!< Value of Private GATT Proxy state */

    struct k_delayed_work update_timer;                 /*!< Timer for update the random field of private beacon */
} esp_ble_mesh_prb_srv_t;

/** Parameter of private Beacon Set */
typedef struct {
    uint8_t private_beacon;                             /*!< New Private Beacon state */
    bool    is_effect;                                  /*!< Decide if update_interval exists */
    uint8_t update_interval;                            /*!< New Random Update Interval Steps state */
} esp_ble_mesh_priv_beacon_set_t;

/** Parameter of Private GATT Proxy Set */
typedef struct {
    uint8_t private_gatt_proxy;                         /*!< New Private GATT Proxy state */
} esp_ble_mesh_priv_gatt_proxy_set_t;

/** Parameter of Private node identity Get */
typedef struct {
    uint16_t net_idx;                                   /*!< Index of the NetKey */
} esp_ble_mesh_priv_node_id_get_t;

/** Parameter of Private node identity Set */
typedef struct {
    uint16_t net_idx;                                   /*!< Index of the NetKey */
    uint8_t  private_node_id;                           /*!< New Private Node Identity state */
} esp_ble_mesh_priv_node_id_set_t;

/**
 * @brief Mesh Private Beacon Client model message union
 */
typedef union {
    esp_ble_mesh_priv_beacon_set_t     priv_beacon_set;     /*!< ESP_BLE_MESH_MODEL_OP_PRIV_BEACON_SET. */
    esp_ble_mesh_priv_gatt_proxy_set_t priv_gatt_proxy_set; /*!< ESP_BLE_MESH_MODEL_OP_PRIV_GATT_PROXY_SET. */
    esp_ble_mesh_priv_node_id_get_t    priv_node_id_get;    /*!< ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_GET. */
    esp_ble_mesh_priv_node_id_set_t    priv_node_id_set;    /*!< ESP_BLE_MESH_MODEL_OP_PRIV_NODE_IDENTITY_SET. */
} esp_ble_mesh_prb_client_msg_t;

/** Parameter of Private Beacon Status */
typedef struct {
    uint8_t private_beacon;                             /*!< Current value of the Private Beacon state */
    uint8_t update_interval;                            /*!< Current value of the Random Update Interval Steps state */
} esp_ble_mesh_priv_beacon_status_cb_t;

/** Parameter of Private GATT Proxy Status */
typedef struct {
    uint8_t private_gatt_proxy;                         /*!< Private GATT Proxy state */
} esp_ble_mesh_priv_gatt_proxy_status_cb_t;

/** Parameters of Private Node Identity Status */
typedef struct {
    uint8_t  status;                                    /*!< Status Code for the requesting message */
    uint16_t net_idx;                                   /*!< Index of the NetKey */
    uint8_t  private_node_id;                           /*!< Private Node Identity state */
} esp_ble_mesh_priv_node_identity_status_cb_t;

/**
 * @brief Private Beacon Client Model received message union
 */
typedef union {
    esp_ble_mesh_priv_beacon_status_cb_t        priv_beacon_status;     /*!< The private beacon status value */
    esp_ble_mesh_priv_gatt_proxy_status_cb_t    priv_gatt_proxy_status; /*!< The private gatt proxy status value */
    esp_ble_mesh_priv_node_identity_status_cb_t priv_node_id_status;    /*!< The private node identity status value */
} esp_ble_mesh_prb_client_recv_cb_t;

/** Result of sending Bridge Configuration Client messages */
typedef struct {
    int err_code;                                       /*!< Result of sending a message */
} esp_ble_mesh_prb_client_send_cb_t;

/** Mesh Private Beacon Client Model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t  *params;        /*!< The client common parameters. */
    /** Union of Private Beacon Client callback */
    union {
        esp_ble_mesh_prb_client_send_cb_t send;         /*!< Result of sending a message */
        esp_ble_mesh_prb_client_recv_cb_t recv;         /*!< The private beacon message status callback values */
    };
} esp_ble_mesh_prb_client_cb_param_t;                   /*!< Mesh Private Beacon Client Model callback parameters */

/** This enum value is the event of Private Beacon Client Model */
typedef enum {
    ESP_BLE_MESH_PRB_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_PRB_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_PRB_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_PRB_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_PRB_CLIENT_EVT_MAX,
} esp_ble_mesh_prb_client_cb_event_t;

/**
 * @brief Mesh Private Beacon Server model related context.
 */
/** Parameters of Private Beacon Set. */
typedef struct {
    uint8_t private_beacon;                             /*!< Private Beacon state */
    bool    is_effect;                                  /*!< Decide whether update_interval effect */
    uint8_t update_interval;                            /*!< Random Update Interval Steps state */
} esp_ble_mesh_state_change_priv_beacon_set_t;

/** Parameters of Private GATT Proxy Set. */
typedef struct {
    uint8_t private_gatt_proxy;                         /*!< Private GATT Proxy state */
} esp_ble_mesh_state_change_priv_gatt_proxy_set_t;

/** Parameters of Private Node Identity Set. */
typedef struct {
    uint16_t net_idx;                                   /*!< Index of the NetKey */
    uint8_t  private_node_id;                           /*!< Private Node Identity state */
} esp_ble_mesh_state_change_priv_node_id_set_t;

/**
 * @brief Mesh Private Beacon Server model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_priv_beacon_set_t     priv_beacon_set;        /*!< Private Beacon Set */
    esp_ble_mesh_state_change_priv_gatt_proxy_set_t priv_gatt_proxy_set;    /*!< Private GATT Proxy Set */
    esp_ble_mesh_state_change_priv_node_id_set_t    priv_node_id_set;       /*!< Private Node Identity Set */
} esp_ble_mesh_prb_server_state_change_t;

/**
 * @brief Private Beacon Server model callback value union
 */
typedef union {
    esp_ble_mesh_prb_server_state_change_t state_change;    /*!< ESP_BLE_MESH_PRB_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_prb_server_cb_value_t;

/** Private Beacon Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                       /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                         /*!< Context of the received message */
    esp_ble_mesh_prb_server_cb_value_t value;           /*!< Value of the received private beacon messages */
} esp_ble_mesh_prb_server_cb_param_t;

/** This enum value is the event of Private Beacon Server model */
typedef enum {
    ESP_BLE_MESH_PRB_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_PRB_SERVER_EVT_MAX,
} esp_ble_mesh_prb_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Private Beacon Client and Server Model functions.
 */

/**
 * @brief   Private Beacon Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_prb_client_cb_t)(esp_ble_mesh_prb_client_cb_event_t event,
                                              esp_ble_mesh_prb_client_cb_param_t *param);

/**
 * @brief   Private Beacon Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_prb_server_cb_t)(esp_ble_mesh_prb_server_cb_event_t event,
                                              esp_ble_mesh_prb_server_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Private Beacon Client Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_prb_client_callback(esp_ble_mesh_prb_client_cb_t callback);

/**
 * @brief         Register BLE Mesh Private Beacon Server Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_prb_server_callback(esp_ble_mesh_prb_server_cb_t callback);

/**
 * @brief         Get/Set the value of Private Beacon Server Model states using the corresponding messages of Private Beacon Client Model.
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     msg:       Pointer to Mesh Private Beacon Client message.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_prb_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_prb_client_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_PRB_MODEL_API_H_ */
