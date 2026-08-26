/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_ODP_MODEL_API_H_
#define _ESP_BLE_MESH_ODP_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_OD_PRIV_PROXY_GET     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x69)
#define ESP_BLE_MESH_MODEL_OP_OD_PRIV_PROXY_SET     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6A)
#define ESP_BLE_MESH_MODEL_OP_OD_PRIV_PROXY_STATUS  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6B)

/** @def    ESP_BLE_MESH_MODEL_ODP_SRV
 *
 *  @brief  Define a new On-Demand Private Proxy Config Server model.
 *
 *  @note   The On-Demand Private Proxy Server model is used to represent the
 *          ability to enable advertising with Private Network Identity type
 *          of a node. This model extends the Mesh Private Beacon Server model.
 *          When this model is present on an element, the corresponding
 *          Solicitation PDU RPL Configuration Server model shall also be present.
 *          The model shall be supported by a primary element and shall not be
 *          supported by any secondary elements.
 *
 *  @param  srv_data Pointer to a unique On-Demand Private Proxy Config Server
 *                   model user_data.
 *
 *  @return New On-Demand Private Proxy Config Server model instance.
 */
#define ESP_BLE_MESH_MODEL_ODP_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_ODP_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_ODP_CLI
 *
 *  @brief  Define a new On-Demand Private Proxy Config Client model.
 *
 *  @note   The model shall be supported by a primary element and shall not be
 *          supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique On-Demand Private Proxy Config Client
 *                   model user_data.
 *
 *  @return New On-Demand Private Proxy Config Client model instance.
 */
#define ESP_BLE_MESH_MODEL_ODP_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_ODP_CLI, \
                               NULL, NULL, cli_data)

/** On-Demand Private Proxy Config Server model context */
typedef struct {
    /** Pointer to On-Demand Private Proxy Config Server model */
    esp_ble_mesh_model_t *model;

    /** Duration in seconds of the interval during which advertising
     *  with Private Network Identity type is enabled after receiving
     *  a Solicitation PDU or after a client disconnect.
     *  Note: Binding with the Private GATT Proxy state.
     */
    uint8_t on_demand_private_gatt_proxy;
} esp_ble_mesh_odp_srv_t;

/** Parameter of On-Demand Private Proxy Set */
typedef struct {
    uint8_t gatt_proxy;                                 /*!< On-Demand Private GATT Proxy */
} esp_ble_mesh_od_priv_proxy_set_t;

/**
 * @brief On-Demand Private Proxy Client model message union
 */
typedef union {
    esp_ble_mesh_od_priv_proxy_set_t od_priv_proxy_set; /*!< For ESP_BLE_MESH_MODEL_OP_OD_PRIV_PROXY_SET */
} esp_ble_mesh_odp_client_msg_t;

/** Parameter of On-Demand Private Proxy Status */
typedef struct {
    uint8_t gatt_proxy;                                 /*!< On-Demand Private GATT Proxy */
} esp_ble_mesh_od_priv_proxy_status_t;

/** Result of sending On-Demand Private Proxy Client messages */
typedef struct {
    int err_code;                                       /*!< Result of sending a message */
} esp_ble_mesh_odp_client_send_cb_t;

/**
 * @brief On-Demand Private Proxy Client model received message union
 */
typedef union {
    esp_ble_mesh_od_priv_proxy_status_t od_priv_proxy_status; /*!< For ESP_BLE_MESH_MODEL_OP_OD_PRIV_PROXY_STATUS */
} esp_ble_mesh_odp_client_recv_cb_t;

/** On-Demand Private Proxy Config Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params;         /*!< Client common parameters, used by all events */
    /** Union of ODP Client callback */
    union {
        esp_ble_mesh_odp_client_send_cb_t send;         /*!< Result of sending a message */
        esp_ble_mesh_odp_client_recv_cb_t recv;         /*!< Parameters of received status message */
    };
} esp_ble_mesh_odp_client_cb_param_t;                   /*!< On-Demand Private Proxy Config Client model callback parameters */

/** This enum value is the event of On-Demand Private Proxy Config Client model */
typedef enum {
    ESP_BLE_MESH_ODP_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_ODP_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_ODP_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_ODP_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_ODP_CLIENT_EVT_MAX,
} esp_ble_mesh_odp_client_cb_event_t;

/**
 * @brief On-Demand Private Proxy Config Server model related context.
 */

/**
 * @brief On-Demand Private Proxy Config Server model state change value union
 */
typedef union {
    uint8_t dummy;                                      /*!< Event not used currently */
} esp_ble_mesh_odp_server_state_change_t;

/**
 * @brief On-Demand Private Proxy Config Server model callback value union
 */
typedef union {
    esp_ble_mesh_odp_server_state_change_t state_change;    /*!< For ESP_BLE_MESH_ODP_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_odp_server_cb_value_t;

/** On-Demand Private Proxy Config Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                       /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                         /*!< Context of the received message */
    esp_ble_mesh_odp_server_cb_value_t value;           /*!< Value of the received configuration messages */
} esp_ble_mesh_odp_server_cb_param_t;

/** This enum value is the event of On-Demand Private Proxy Config Server model */
typedef enum {
    ESP_BLE_MESH_ODP_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_ODP_SERVER_EVT_MAX,
} esp_ble_mesh_odp_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh On-Demand Private Proxy Config client and server model functions.
 */

/**
 * @brief   On-Demand Private Proxy Config Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_odp_client_cb_t)(esp_ble_mesh_odp_client_cb_event_t event,
                                              esp_ble_mesh_odp_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh On-Demand Private Proxy Config Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_odp_client_callback(esp_ble_mesh_odp_client_cb_t callback);

/**
 * @brief       Get the value of On-Demand Private Proxy Config Server model state with the corresponding get message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to On-Demand Private Proxy Config Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_odp_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_odp_client_msg_t *msg);

/**
 * @brief   On-Demand Private Proxy Config Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_odp_server_cb_t)(esp_ble_mesh_odp_server_cb_event_t event,
                                              esp_ble_mesh_odp_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh On-Demand Private Proxy Config Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_odp_server_callback(esp_ble_mesh_odp_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_ODP_MODEL_API_H_ */
