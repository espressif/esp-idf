/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_SAR_MODEL_API_H_
#define _ESP_BLE_MESH_SAR_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_SAR_TRANSMITTER_GET       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6C)
#define ESP_BLE_MESH_MODEL_OP_SAR_TRANSMITTER_SET       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6D)
#define ESP_BLE_MESH_MODEL_OP_SAR_TRANSMITTER_STATUS    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6E)
#define ESP_BLE_MESH_MODEL_OP_SAR_RECEIVER_GET          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x6F)
#define ESP_BLE_MESH_MODEL_OP_SAR_RECEIVER_SET          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x70)
#define ESP_BLE_MESH_MODEL_OP_SAR_RECEIVER_STATUS       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x71)

/** @def    ESP_BLE_MESH_MODEL_SAR_SRV
 *
 *  @brief  Define a new SAR Configuration Server model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  srv_data Pointer to a unique SAR Configuration Server model user_data.
 *
 *  @return New SAR Configuration Server model instance.
 */
#define ESP_BLE_MESH_MODEL_SAR_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SAR_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SAR_CLI
 *
 *  @brief  Define a new SAR Configuration Client model.
 *
 *  @note   If supported, the model shall be supported by the primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique SAR Configuration Client model user_data.
 *
 *  @return New SAR Configuration Client model instance.
 */
#define ESP_BLE_MESH_MODEL_SAR_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SAR_CLI, \
                               NULL, NULL, cli_data)

/** SAR Configuration Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                            /*!< Pointer to SAR Configuration Server model */
} esp_ble_mesh_sar_srv_t;

/** Parameters of SAR Transmitter Set */
typedef struct {
    uint8_t sar_segment_interval_step:4,                    /*!< SAR Segment Interval Step state value */
            sar_unicast_retrans_count:4;                    /*!< SAR Unicast Retransmissions Count state */
    uint8_t sar_unicast_retrans_without_progress_count:4,   /*!< SAR Unicast Retransmissions Without Progress Count state */
            sar_unicast_retrans_interval_step:4;            /*!< SAR Unicast Retransmissions Interval Step state */
    uint8_t sar_unicast_retrans_interval_increment:4,       /*!< SAR Unicast Retransmissions Interval Increment state */
            sar_multicast_retrans_count:4;                  /*!< SAR Multicast Retransmissions Count state */
    uint8_t sar_multicast_retrans_interval_step:4;          /*!< SAR Multicast Retransmissions Interval state */
} esp_ble_mesh_sar_transmitter_set_t;

/** Parameters of SAR Receiver Set */
typedef struct {
    uint8_t sar_segments_threshold:5,                       /*!< SAR Segments Threshold state */
            sar_ack_delay_increment:3;                      /*!< SAR Acknowledgment Delay Increment state */
    uint8_t sar_discard_timeout:4,                          /*!< SAR Discard Timeout state */
            sar_receiver_segment_interval_step:4;           /*!< SAR Receiver Segment Interval Step state */
    uint8_t sar_ack_retrans_count:4;                        /*!< SAR Acknowledgment Retransmissions Count state */
} esp_ble_mesh_sar_receiver_set_t;

/**
 * @brief SAR Configuration Client model message union
 */
typedef union {
    esp_ble_mesh_sar_transmitter_set_t sar_transmitter_set; /*!< For ESP_BLE_MESH_MODEL_OP_SAR_TRANSMITTER_SET */
    esp_ble_mesh_sar_receiver_set_t    sar_receiver_set;    /*!< For ESP_BLE_MESH_MODEL_OP_SAR_RECEIVER_SET */
} esp_ble_mesh_sar_client_msg_t;

/** Parameters of SAR Transmitter Status */
typedef struct {
    uint8_t sar_segment_interval_step:4,                    /*!< SAR Segment Interval Step state value */
            sar_unicast_retrans_count:4;                    /*!< SAR Unicast Retransmissions Count state */
    uint8_t sar_unicast_retrans_without_progress_count:4,   /*!< SAR Unicast Retransmissions Without Progress Count state */
            sar_unicast_retrans_interval_step:4;            /*!< SAR Unicast Retransmissions Interval Step state */
    uint8_t sar_unicast_retrans_interval_increment:4,       /*!< SAR Unicast Retransmissions Interval Increment state */
            sar_multicast_retrans_count:4;                  /*!< SAR Multicast Retransmissions Count state */
    uint8_t sar_multicast_retrans_interval_step:4;          /*!< SAR Multicast Retransmissions Interval state */
} esp_ble_mesh_sar_transmitter_status_t;

/** Parameters of SAR Receiver Status */
typedef struct {
    uint8_t sar_segments_threshold:5,                       /*!< SAR Segments Threshold state */
            sar_ack_delay_increment:3;                      /*!< SAR Acknowledgment Delay Increment state */
    uint8_t sar_discard_timeout:4,                          /*!< SAR Discard Timeout state */
            sar_receiver_segment_interval_step:4;           /*!< SAR Receiver Segment Interval Step state */
    uint8_t sar_ack_retrans_count:4;                        /*!< SAR Acknowledgment Retransmissions Count state */
} esp_ble_mesh_sar_receiver_status_t;

/** Result of sending SAR Configuration Client messages */
typedef struct {
    int err_code; /*!< Result of sending a message */
} esp_ble_mesh_sar_client_send_cb_t;

/**
 * @brief SAR Configuration Client model received message union
 */
typedef union {
    esp_ble_mesh_sar_transmitter_status_t sar_transmitter_status;   /*!< For ESP_BLE_MESH_MODEL_OP_SAR_TRANSMITTER_STATUS */
    esp_ble_mesh_sar_receiver_status_t    sar_receiver_status;      /*!< For ESP_BLE_MESH_MODEL_OP_SAR_RECEIVE_STATUS */
} esp_ble_mesh_sar_client_recv_cb_t;

/** SAR Configuration Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params;             /*!< Client common parameters, used by all events. */
    /** Union of SAR Client callback */
    union {
        esp_ble_mesh_sar_client_send_cb_t send;             /*!< Result of sending a message */
        esp_ble_mesh_sar_client_recv_cb_t recv;             /*!< Parameters of received status message */
    };
} esp_ble_mesh_sar_client_cb_param_t;                       /*!< SAR Configuration Client model callback parameters */

/** This enum value is the event of SAR Configuration Client model */
typedef enum {
    ESP_BLE_MESH_SAR_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_SAR_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_SAR_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_SAR_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_SAR_CLIENT_EVT_MAX,
} esp_ble_mesh_sar_client_cb_event_t;

/**
 * @brief SAR Configuration Server model related context.
 */

/**
 * @brief SAR Configuration Server model state change value union
 */
typedef union {
    uint8_t dummy;                                          /*!< Event not used currently */
} esp_ble_mesh_sar_server_state_change_t;

/**
 * @brief SAR Configuration Server model callback value union
 */
typedef union {
    esp_ble_mesh_sar_server_state_change_t state_change;    /*!< For ESP_BLE_MESH_SAR_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_sar_server_cb_value_t;

/** SAR Configuration Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                           /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                             /*!< Context of the received message */
    esp_ble_mesh_sar_server_cb_value_t value;               /*!< Value of the received configuration messages */
} esp_ble_mesh_sar_server_cb_param_t;

/** This enum value is the event of SAR Configuration Server model */
typedef enum {
    ESP_BLE_MESH_SAR_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_SAR_SERVER_EVT_MAX,
} esp_ble_mesh_sar_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh SAR Configuration client and server model functions.
 */

/**
 * @brief   SAR Configuration Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_sar_client_cb_t)(esp_ble_mesh_sar_client_cb_event_t event,
                                              esp_ble_mesh_sar_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh SAR Configuration Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_sar_client_callback(esp_ble_mesh_sar_client_cb_t callback);

/**
 * @brief       Get the value of SAR Configuration Server model state with the corresponding get message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to SAR Configuration Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_sar_client_send(esp_ble_mesh_client_common_param_t *params,
                                           esp_ble_mesh_sar_client_msg_t *msg);

/**
 * @brief   SAR Configuration Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_sar_server_cb_t)(esp_ble_mesh_sar_server_cb_event_t event,
                                              esp_ble_mesh_sar_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh SAR Configuration Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_sar_server_callback(esp_ble_mesh_sar_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_SAR_MODEL_API_H_ */
