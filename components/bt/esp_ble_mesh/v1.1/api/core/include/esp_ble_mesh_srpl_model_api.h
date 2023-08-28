/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_SRPL_MODEL_API_H_
#define _ESP_BLE_MESH_SRPL_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_SRPL_ITEMS_CLEAR          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x78)
#define ESP_BLE_MESH_MODEL_OP_SRPL_ITEMS_CLEAR_UNACK    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x79)
#define ESP_BLE_MESH_MODEL_OP_SRPL_ITEMS_STATUS         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x7A)

/** @def    ESP_BLE_MESH_MODEL_SRPL_SRV
 *
 *  @brief  Define a new Solicitation PDU RPL Configuration Server model.
 *
 *  @note   The Solicitation PDU RPL Configuration Server model extends
 *          the On-Demand Private Proxy Server model.
 *          If the model is supported, the model shall be supported by a
 *          primary element and shall not be supported by any secondary
 *          elements.
 *
 *  @param  srv_data Pointer to a unique Solicitation PDU RPL Configuration Server
 *                   model user_data.
 *
 *  @return New Solicitation PDU RPL Configuration Server model instance.
 */
#define ESP_BLE_MESH_MODEL_SRPL_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SRPL_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SRPL_CLI
 *
 *  @brief  Define a new Solicitation PDU RPL Configuration Client model.
 *
 *  @note   If supported, the model shall be supported by the primary
 *          element and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique Solicitation PDU RPL Configuration Client
 *                   model user_data.
 *
 *  @return New Solicitation PDU RPL Configuration Client model instance.
 */
#define ESP_BLE_MESH_MODEL_SRPL_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SRPL_CLI, \
                               NULL, NULL, cli_data)

/** Solicitation PDU RPL Configuration Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to Solicitation PDU RPL Configuration Server model */

    /* This model does not define any states. */
} esp_ble_mesh_srpl_srv_t;

/** Parameter of Solicitation PDU RPL Items Clear */
typedef struct {
    esp_ble_mesh_uar_t addr_range;                      /*!< Unicast address range */
} esp_ble_mesh_srpl_items_clear_t;

/**
 * @brief Solicitation PDU RPL Configuration Client model message union
 */
typedef union {
    esp_ble_mesh_srpl_items_clear_t srpl_items_clear;   /*!< For ESP_BLE_MESH_MODEL_OP_SRPL_ITEMS_CLEAR */
} esp_ble_mesh_srpl_client_msg_t;

/** Parameter of Solicitation PDU RPL Items Clear Status */
typedef struct {
    esp_ble_mesh_uar_t addr_range;                      /*!< Unicast address range */
} esp_ble_mesh_srpl_items_status_t;

/** Result of sending Solicitation PDU RPL Configuration Client messages */
typedef struct {
    int err_code;                                       /*!< Result of sending a message */
} esp_ble_mesh_srpl_client_send_cb_t;

/**
 * @brief Solicitation PDU RPL Configuration Client model received message union
 */
typedef union {
    esp_ble_mesh_srpl_items_status_t srpl_items_status; /*!< For ESP_BLE_MESH_MODEL_OP_SRPL_ITEMS_STATUS */
} esp_ble_mesh_srpl_client_recv_cb_t;

/** Solicitation PDU RPL Configuration Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params;         /*!< Client common parameters, used by all events. */
    /** Union of SRPL Client callback */
    union {
        esp_ble_mesh_srpl_client_send_cb_t send;        /*!< Result of sending a message */
        esp_ble_mesh_srpl_client_recv_cb_t recv;        /*!< Parameters of received status message */
    };
} esp_ble_mesh_srpl_client_cb_param_t;                  /*!< Solicitation PDU RPL Configuration Client model callback parameters */

/** This enum value is the event of Solicitation PDU RPL Configuration Client model */
typedef enum {
    ESP_BLE_MESH_SRPL_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_SRPL_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_SRPL_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_SRPL_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_SRPL_CLIENT_EVT_MAX,
} esp_ble_mesh_srpl_client_cb_event_t;

/**
 * @brief Solicitation PDU RPL Configuration Server model related context.
 */

/**
 * @brief Solicitation PDU RPL Configuration Server model state change value union
 */
typedef union {
    uint8_t dummy;                                      /*!< Currently this event is not used. */
} esp_ble_mesh_srpl_server_state_change_t;

/**
 * @brief Solicitation PDU RPL Configuration Server model callback value union
 */
typedef union {
    esp_ble_mesh_srpl_server_state_change_t state_change;   /*!< ESP_BLE_MESH_SRPL_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_srpl_server_cb_value_t;

/** Solicitation PDU RPL Configuration Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                       /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                         /*!< Context of the received message */
    esp_ble_mesh_srpl_server_cb_value_t value;          /*!< Value of the received configuration messages */
} esp_ble_mesh_srpl_server_cb_param_t;

/** This enum value is the event of Solicitation PDU RPL Configuration Server model */
typedef enum {
    ESP_BLE_MESH_SRPL_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_SRPL_SERVER_EVT_MAX,
} esp_ble_mesh_srpl_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Solicitation PDU RPL Configuration client and server model functions.
 */

/**
 * @brief   Solicitation PDU RPL Configuration Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_srpl_client_cb_t)(esp_ble_mesh_srpl_client_cb_event_t event,
                                               esp_ble_mesh_srpl_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Solicitation PDU RPL Configuration Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_srpl_client_callback(esp_ble_mesh_srpl_client_cb_t callback);

/**
 * @brief       Set the value of Solicitation PDU RPL Configuration Server model state with the corresponding set message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to Solicitation PDU RPL Configuration Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_srpl_client_send(esp_ble_mesh_client_common_param_t *params,
                                        esp_ble_mesh_srpl_client_msg_t *msg);


/**
 * @brief   Solicitation PDU RPL Configuration Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_srpl_server_cb_t)(esp_ble_mesh_srpl_server_cb_event_t event,
                                               esp_ble_mesh_srpl_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Solicitation PDU RPL Configuration Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_srpl_server_callback(esp_ble_mesh_srpl_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_SRPL_MODEL_API_H_ */
