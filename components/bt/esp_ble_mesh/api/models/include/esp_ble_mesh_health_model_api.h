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

#ifndef _ESP_BLE_MESH_HEALTH_MODEL_API_H_
#define _ESP_BLE_MESH_HEALTH_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

/** @def    ESP_BLE_MESH_MODEL_HEALTH_SRV
 *
 *  @brief  Define a new Health Server Model.
 *
 *  @note   The Health Server Model can only be included by a Primary Element.
 *
 *  @param  srv Pointer to the unique struct esp_ble_mesh_health_srv_t.
 *  @param  pub Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *
 *  @return New Health Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_HEALTH_SRV(srv, pub)                           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_HEALTH_SRV,          \
                           NULL, pub, srv)

/** @def    ESP_BLE_MESH_MODEL_HEALTH_CLI
 *
 *  @brief  Define a new Health Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Health Client Model.
 *
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Health Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_HEALTH_CLI(cli_data)                           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_HEALTH_CLI,          \
                           NULL, NULL, cli_data)

typedef struct {
    /* Fetch current faults */
    int (*fault_get_cur)(esp_ble_mesh_model_t *model, uint8_t *test_id,
                         uint16_t *company_id, uint8_t *faults, uint8_t *fault_count);

    /* Fetch registered faults */
    int (*fault_get_reg)(esp_ble_mesh_model_t *model, uint16_t company_id,
                         uint8_t *test_id, uint8_t *faults, uint8_t *fault_count);

    /* Clear registered faults */
    int (*fault_clear)(esp_ble_mesh_model_t *model, uint16_t company_id);

    /* Run a specific test */
    int (*fault_test)(esp_ble_mesh_model_t *model, uint8_t test_id, uint16_t company_id);

    /* Attention on */
    void (*attn_on)(esp_ble_mesh_model_t *model);

    /* Attention off */
    void (*attn_off)(esp_ble_mesh_model_t *model);
} esp_ble_mesh_health_srv_cb_t;

/** ESP BLE Mesh Health Server Model Context */
typedef struct {
    esp_ble_mesh_model_t *model;

    /* Optional callback struct */
    const esp_ble_mesh_health_srv_cb_t *cb;

    /* Attention Timer state */
    struct k_delayed_work attn_timer;
} esp_ble_mesh_health_srv_t;

/** BLE Mesh Health Client Model fault get Context */
typedef struct {
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
} esp_ble_mesh_health_fault_get_t;

/** Mesh Health Client Model attention set Context */
typedef struct {
    uint8_t attention;                      /*!< Value of the Attention Timer state */
} esp_ble_mesh_health_attention_set_t;

/** Mesh Health client Model period set Context */
typedef struct {
    uint8_t fast_period_divisor;            /*!< Divider for the Publish Period */
} esp_ble_mesh_health_period_set_t;

/** BLE Mesh Health Client Model fault test Context */
typedef struct {
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
    uint8_t  test_id;                       /*!< ID of a specific test to be performed */
} esp_ble_mesh_health_fault_test_t;

/** BLE Mesh Health Client Model fault clear Context */
typedef struct {
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
} esp_ble_mesh_health_fault_clear_t;

/**
 * @brief For ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET
 *            ESP_BLE_MESH_MODEL_OP_ATTENTION_GET
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET
 * the get_state parameter in the esp_ble_mesh_health_client_get_state function should not be set to NULL.
 */
typedef union {
    esp_ble_mesh_health_fault_get_t fault_get;          /*!< For ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET. */
} esp_ble_mesh_health_client_get_state_t;

/**
 * @brief For ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET
 *            ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK
 *            ESP_BLE_MESH_MODEL_OP_ATTENTION_SET
 *            ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK
 * the set_state parameter in the esp_ble_mesh_health_client_set_state function should not be set to NULL.
 */
typedef union {
    esp_ble_mesh_health_attention_set_t attention_set;    /*!< For ESP_BLE_MESH_MODEL_OP_ATTENTION_SET or ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK. */
    esp_ble_mesh_health_period_set_t    period_set;       /*!< For ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET or ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK. */
    esp_ble_mesh_health_fault_test_t    fault_test;       /*!< For ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST or ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK. */
    esp_ble_mesh_health_fault_clear_t   fault_clear;      /*!< For ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR or ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK. */
} esp_ble_mesh_health_client_set_state_t;

typedef struct {
    uint8_t  test_id;                       /*!< ID of a most recently performed test */
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
    struct net_buf_simple *fault_array;     /*!< FaultArray field contains a sequence of 1-octet fault values */
} esp_ble_mesh_health_current_status_cb_t;

typedef struct {
    uint8_t  test_id;                       /*!< ID of a most recently performed test */
    uint16_t company_id;                    /*!< Bluetooth assigned 16-bit Company ID */
    struct net_buf_simple *fault_array;     /*!< FaultArray field contains a sequence of 1-octet fault values */
} esp_ble_mesh_health_fault_status_cb_t;

typedef struct {
    uint8_t fast_period_divisor;            /*!< Divider for the Publish Period */
} esp_ble_mesh_health_period_status_cb_t;

typedef struct {
    uint8_t attention;                      /*!< Value of the Attention Timer state */
} esp_ble_mesh_health_attention_status_cb_t;

typedef union {
    esp_ble_mesh_health_current_status_cb_t   current_status;       /*!< The health current status value */
    esp_ble_mesh_health_fault_status_cb_t     fault_status;         /*!< The health fault status value */
    esp_ble_mesh_health_period_status_cb_t    period_status;        /*!< The health period status value */
    esp_ble_mesh_health_attention_status_cb_t attention_status;     /*!< The health attention status value */
} esp_ble_mesh_health_client_common_cb_param_t;

typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t          *params;    /*!< The client common parameters. */
    esp_ble_mesh_health_client_common_cb_param_t status_cb; /*!< The health message status callback values */
} esp_ble_mesh_health_client_cb_param_t;

typedef enum {
    ESP_BLE_MESH_HEALTH_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_HEALTH_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_HEALTH_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_HEALTH_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_HEALTH_CLIENT_EVT_MAX,
} esp_ble_mesh_health_client_cb_event_t;

typedef struct {
    int error_code;                                       /*!< Appropriate error code */
} esp_ble_mesh_health_server_cb_param_t;

typedef enum {
    ESP_BLE_MESH_HEALTH_SERVER_FAULT_UPDATE_COMPLETE_EVT,
    ESP_BLE_MESH_HEALTH_SERVER_EVT_MAX,
} esp_ble_mesh_health_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Health Client and Server Model function.
 */

/** @brief: event, event code of Health Client Model event; param, parameters of Health Client Model event) */
typedef void (* esp_ble_mesh_health_client_cb_t)(esp_ble_mesh_health_client_cb_event_t event,
        esp_ble_mesh_health_client_cb_param_t *param);

/** @brief: event, event code of Health Server Model event; param, parameters of Health Server Model event) */
typedef void (* esp_ble_mesh_health_server_cb_t)(esp_ble_mesh_health_server_cb_event_t event,
        esp_ble_mesh_health_server_cb_param_t *param);

/**
 * @brief         Register BLE Mesh Health Model callback, the callback will report Health Client & Server Model events.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_health_client_callback(esp_ble_mesh_health_client_cb_t callback);

/**
 * @brief         Register BLE Mesh Health Server Model callback.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_health_server_callback(esp_ble_mesh_health_server_cb_t callback);

/**
 * @brief         This function is called to get the Health Server states using the Health Client Model get messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to (@ref esp_ble_mesh_opcode_health_client_get_t).
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     get_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_health_client_get_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_health_client_get_state_t *get_state);

/**
 * @brief         This function is called to set the Health Server states using the Health Client Model set messages.
 *
 * @note          If you want to find the opcodes and corresponding meanings accepted by this API,
 *                please refer to (@ref esp_ble_mesh_opcode_health_client_set_t).
 *
 * @param[in]     params:    Pointer to BLE Mesh common client parameters.
 * @param[in]     set_state: Pointer to a union, each kind of opcode corresponds to one structure inside.
 *                           Shall not be set to NULL.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_health_client_set_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_health_client_set_state_t *set_state);

/**
 * @brief         This function is called by the Health Server Model to start to publish its Current Health Fault.
 *
 * @param[in]     element: The element to which the Health Server Model belongs.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_health_server_fault_update(esp_ble_mesh_elem_t *element);

#endif /** _ESP_BLE_MESH_HEALTH_MODEL_API_H_ */
