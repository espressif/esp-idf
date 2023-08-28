/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_LCD_MODEL_API_H_
#define _ESP_BLE_MESH_LCD_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_GET       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x74)
#define ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_STATUS    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x75)
#define ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_GET       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x76)
#define ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_STATUS    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x77)

/** @def    ESP_BLE_MESH_MODEL_LCD_SRV
 *
 *  @brief  Define a new Large Composition Data Server model.
 *
 *  @note   If supported, the model shall be supported by a primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  srv_data Pointer to a unique Large Composition Data Server model user_data.
 *
 *  @return New Large Composition Data Server model instance.
 */
#define ESP_BLE_MESH_MODEL_LCD_SRV(srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LCD_SRV, \
                               NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LCD_CLI
 *
 *  @brief  Define a new Large Composition Data Client model.
 *
 *  @note   If supported, the model shall be supported by the primary element
 *          and shall not be supported by any secondary elements.
 *
 *  @param  cli_data Pointer to a unique Large Composition Data Client model user_data.
 *
 *  @return New Large Composition Data Client model instance.
 */
#define ESP_BLE_MESH_MODEL_LCD_CLI(cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LCD_CLI, \
                               NULL, NULL, cli_data)

/** Large Composition Data Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to Large Composition Data Server model */
} esp_ble_mesh_lcd_srv_t;

/** Parameters of Large Composition Data Get */
typedef struct {
    uint8_t  page;                                      /*!< Page number of the Composition Data */
    uint16_t offset;                                    /*!< Offset within the page */
} esp_ble_mesh_large_comp_data_get_t;

/** Parameters of Models Metadata Get */
typedef struct {
    uint8_t  metadata_page;                             /*!< Page number of the Models Metadata */
    uint16_t offset;                                    /*!< Offset within the page  */
} esp_ble_mesh_models_metadata_get_t;

/**
 * @brief Large Composition Data Client model message union
 */
typedef union {
    esp_ble_mesh_large_comp_data_get_t large_comp_data_get; /*!< For ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_GET */
    esp_ble_mesh_models_metadata_get_t models_metadata_get; /*!< For ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_GET */
} esp_ble_mesh_lcd_client_msg_t;

/** Parameters of Large Composition Data Status */
typedef struct {
    uint8_t  page;                                      /*!< Page number of the Composition Data */
    uint16_t offset;                                    /*!< Offset within the page */
    uint16_t total_size;                                /*!< Total size of the page */
    struct net_buf_simple *data;                        /*!< Composition Data for the identified portion of the page */
} esp_ble_mesh_large_comp_data_status_t;

/** Parameters of Models Metadata Data Status */
typedef struct {
    uint8_t  metadata_page;                             /*!< Page number of the Models Metadata */
    uint16_t offset;                                    /*!< Offset within the page */
    uint16_t total_size;                                /*!< Total size of the page */
    struct net_buf_simple *data;                        /*!< Models Metadata for the identified portion of the page */
} esp_ble_mesh_models_metadata_status_t;

/** Result of sending Large Composition Data Client messages */
typedef struct {
    int err_code;                                       /*!< Result of sending a message */
} esp_ble_mesh_lcd_client_send_cb_t;

/**
 * @brief Large Composition Data Client model received message union
 */
typedef union {
    esp_ble_mesh_large_comp_data_status_t large_comp_data_status;   /*!< For ESP_BLE_MESH_MODEL_OP_LARGE_COMP_DATA_STATUS */
    esp_ble_mesh_models_metadata_status_t models_metadata_status;   /*!< For ESP_BLE_MESH_MODEL_OP_MODELS_METADATA_STATUS */
} esp_ble_mesh_lcd_client_recv_cb_t;

/** Large Composition Data Client model callback parameters */
typedef struct {
    esp_ble_mesh_client_common_param_t *params;         /*!< Client common parameters, used by all events. */
    /** Union of LCD Client callback */
    union {
        esp_ble_mesh_lcd_client_send_cb_t send;         /*!< Result of sending a message */
        esp_ble_mesh_lcd_client_recv_cb_t recv;         /*!< Parameters of received status message */
    };
} esp_ble_mesh_lcd_client_cb_param_t;                   /*!< Large Composition Data Client model callback parameters */

/** This enum value is the event of Large Composition Data Client model */
typedef enum {
    ESP_BLE_MESH_LCD_CLIENT_SEND_COMP_EVT,
    ESP_BLE_MESH_LCD_CLIENT_SEND_TIMEOUT_EVT,
    ESP_BLE_MESH_LCD_CLIENT_RECV_RSP_EVT,
    ESP_BLE_MESH_LCD_CLIENT_RECV_PUB_EVT,
    ESP_BLE_MESH_LCD_CLIENT_EVT_MAX,
} esp_ble_mesh_lcd_client_cb_event_t;

/**
 * @brief Large Composition Data Server model related context.
 */

/**
 * @brief Large Composition Data Server model state change value union
 */
typedef union {
    uint8_t dummy;                                      /*!< Event not used currently */
} esp_ble_mesh_lcd_server_state_change_t;

/**
 * @brief Large Composition Data Server model callback value union
 */
typedef union {
    esp_ble_mesh_lcd_server_state_change_t state_change; /*!< For ESP_BLE_MESH_LCD_SERVER_STATE_CHANGE_EVT */
} esp_ble_mesh_lcd_server_cb_value_t;

/** Large Composition Data Server model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;                       /*!< Pointer to the server model structure */
    esp_ble_mesh_msg_ctx_t ctx;                         /*!< Context of the received message */
    esp_ble_mesh_lcd_server_cb_value_t value;           /*!< Value of the received configuration messages */
} esp_ble_mesh_lcd_server_cb_param_t;

/** This enum value is the event of Large Composition Data Server model */
typedef enum {
    ESP_BLE_MESH_LCD_SERVER_STATE_CHANGE_EVT,
    ESP_BLE_MESH_LCD_SERVER_EVT_MAX,
} esp_ble_mesh_lcd_server_cb_event_t;

/**
 *  @brief Large Composition Data client and server model functions.
 */

/**
 * @brief   Large Composition Data Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_lcd_client_cb_t)(esp_ble_mesh_lcd_client_cb_event_t event,
                                              esp_ble_mesh_lcd_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Large Composition Data Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_lcd_client_callback(esp_ble_mesh_lcd_client_cb_t callback);

/**
 * @brief       Get the value of Large Composition Data Server model state with the corresponding get message.
 *
 * @param[in]   params: Pointer to BLE Mesh common client parameters.
 * @param[in]   msg:    Pointer to Large Composition Data Client message.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_lcd_client_send(esp_ble_mesh_client_common_param_t *params,
                                       esp_ble_mesh_lcd_client_msg_t *msg);

/**
 * @brief   Large Composition Data Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_lcd_server_cb_t)(esp_ble_mesh_lcd_server_cb_event_t event,
                                              esp_ble_mesh_lcd_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Large Composition Data Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_lcd_server_callback(esp_ble_mesh_lcd_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_LCD_MODEL_API_H_ */
