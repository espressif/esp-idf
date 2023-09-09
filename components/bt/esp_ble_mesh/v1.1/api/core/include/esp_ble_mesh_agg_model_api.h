/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_AGG_MODEL_API_H_
#define _ESP_BLE_MESH_AGG_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Defines the Opcodes Aggregator message opcode. */
#define ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x72)
#define ESP_BLE_MESH_MODEL_OP_AGG_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x80, 0x73)

/** Defines the status codes for Opcodes Aggregator messages. */
#define ESP_BLE_MESH_AGG_STATUS_SUCCESS                 0x00
#define ESP_BLE_MESH_AGG_STATUS_INVALID_ADDRESS         0x01
#define ESP_BLE_MESH_AGG_STATUS_INVALID_MODEL           0x02
#define ESP_BLE_MESH_AGG_STATUS_WRONG_ACCESS_KEY        0x03
#define ESP_BLE_MESH_AGG_STATUS_WRONG_OPCODE            0x04
#define ESP_BLE_MESH_AGG_STATUS_MSG_NOT_UNDERSTOOD      0x05

/* A message that is not understood includes messages that have
 * one or more of the following conditions:
 * • The application opcode is unknown by the receiving element.
 * • The access message size for the application opcode is incorrect.
 * • The application parameters contain values that are currently
 *   Prohibited.
 */

/** Values of the Length_Format */
#define ESP_BLE_MESH_AGG_ITEM_LENGTH_FORMAT_SHORT       0
#define ESP_BLE_MESH_AGG_ITEM_LENGTH_FORMAT_LONG        1

/** @def    ESP_BLE_MESH_MODEL_AGG_SRV
 *
 *  @brief  Define a new Opcodes Aggregator Server model.
 *
 *  @note   If supported, the Opcodes Aggregator Server model shall be
 *          supported by a primary element.
 *
 *  @param  srv_data Pointer to a unique Opcodes Aggregator Server
 *                   model user_data.
 *
 *  @return New Opcodes Aggregator Server model instance.
 */
#define ESP_BLE_MESH_MODEL_AGG_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_AGG_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_AGG_CLI
 *
 *  @brief  Define a new Opcodes Aggregator Client model.
 *
 *  @note   If supported, the model shall be supported by the primary
 *          element and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique Opcodes Aggregator Client
 *                   model user_data.
 *
 *  @return New Opcodes Aggregator Client model instance.
 */
#define ESP_BLE_MESH_MODEL_AGG_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_AGG_CLI, \
                               NULL, NULL, cli_data)

/** Opcodes Aggregator Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to Opcodes Aggregator Server model */
} esp_ble_mesh_agg_srv_t;                       /*!< Opcodes Aggregator Server model context */

/** Parameters of Aggregator Item */
typedef struct {
    uint16_t length_format:1,                   /*!< 0: Length_Short; 1: Length_Long */
             length:15;                         /*!< Size of Opcode_And_Parameters field */
    const uint8_t *data;                        /*!< Opcode and parameters */
} esp_ble_mesh_agg_item_t;                      /*!< Parameters of Aggregator Item */

/** Parameters of Opcodes Aggregator Sequence */
typedef struct {
    uint16_t element_addr;                      /*!< Element address */
    struct net_buf_simple *items;               /*!< List of items with each item represented as an Aggregator Item */
} esp_ble_mesh_agg_sequence_t;                  /*!< Parameters of Opcodes Aggregator Sequence */

/**
 * @brief Opcodes Aggregator Client model message union
 */
typedef union {
    esp_ble_mesh_agg_sequence_t agg_sequence;   /*!< For ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE */
} esp_ble_mesh_agg_client_msg_t;                /*!< Opcodes Aggregator Client model message union */

/** Parameters of Opcodes Aggregator Status */
typedef struct {
    uint8_t  status;                            /*!< Status of the most recent operation */
    uint16_t element_addr;                      /*!< Element Address */
    struct net_buf_simple *items;               /*!< List of status items with each status item containing an unacknowledged access layer message or empty item (Optional) */
} esp_ble_mesh_agg_status_t;                    /*!< Parameters of Opcodes Aggregator Status */

/** Result of sending Opcodes Aggregator Client messages */
typedef struct {
    int err_code;                               /*!< Result of sending a message */
} esp_ble_mesh_agg_client_send_cb_t;            /*!< Result of sending Opcodes Aggregator Client messages */

/**
 * @brief Opcodes Aggregator Client model received message union
 */
typedef union {
    esp_ble_mesh_agg_status_t agg_status;       /*!< For ESP_BLE_MESH_MODEL_OP_AGG_STATUS */
} esp_ble_mesh_agg_client_recv_cb_t;            /*!< Opcodes Aggregator Client model received message union */

/** Opcodes Aggregator Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params; /*!< Client common parameters, used by all events */
    /** Union of AGG Client callback */
    union {
        esp_ble_mesh_agg_client_send_cb_t send; /*!< Result of sending a message */
        esp_ble_mesh_agg_client_recv_cb_t recv; /*!< Parameters of received status message */
    };
} esp_ble_mesh_agg_client_cb_param_t;           /*!< Opcodes Aggregator Client model callback parameters */

/** This enum value is the event of Opcodes Aggregator Client model */
typedef enum {
    ESP_BLE_MESH_AGG_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_AGG_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_AGG_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_AGG_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_AGG_CLIENT_EVT_MAX,
} esp_ble_mesh_agg_client_cb_event_t;

/**
 * @brief Opcodes Aggregator Server model related context.
 */

/**
 * @brief Opcodes Aggregator Server model received message union
 */
typedef union {
    esp_ble_mesh_agg_sequence_t agg_sequence;   /*!< For ESP_BLE_MESH_MODEL_OP_AGG_SEQUENCE */
} esp_ble_mesh_agg_server_recv_msg_t;           /*!< Opcodes Aggregator Server model received message union */

/** Opcodes Aggregator Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;               /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                 /*!< Context of the received message */
    /** Union of AGG Server callback */
    union {
        esp_ble_mesh_agg_server_recv_msg_t recv;    /*!< Received message callback values */
    };
} esp_ble_mesh_agg_server_cb_param_t;           /*!< Opcodes Aggregator Server model callback parameters */

/** This enum value is the event of Opcodes Aggregator Server model */
typedef enum {
    ESP_BLE_MESH_AGG_SERVER_RECV_MSG_EVT,
    ESP_BLE_MESH_AGG_SERVER_EVT_MAX,
} esp_ble_mesh_agg_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Opcodes Aggregator client and server model functions.
 */

/**
 * @brief   Opcodes Aggregator Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_agg_client_cb_t)(esp_ble_mesh_agg_client_cb_event_t event,
                                              esp_ble_mesh_agg_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Opcodes Aggregator Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_agg_client_callback(esp_ble_mesh_agg_client_cb_t callback);

/**
 * @brief       Set the value of Opcodes Aggregator Server model state with the corresponding set message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to Opcodes Aggregator Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_agg_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_agg_client_msg_t *msg);

/**
 * @brief   Opcodes Aggregator Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_agg_server_cb_t)(esp_ble_mesh_agg_server_cb_event_t event,
                                              esp_ble_mesh_agg_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Opcodes Aggregator Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_agg_server_callback(esp_ble_mesh_agg_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_AGG_MODEL_API_H_ */
