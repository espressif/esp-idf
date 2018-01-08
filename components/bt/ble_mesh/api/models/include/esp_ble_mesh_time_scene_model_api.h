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

/** @file
 *  @brief Bluetooth Mesh Time and Scene Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_
#define _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_

#include "time_scene_client.h"
#include "esp_ble_mesh_defs.h"

/** @def    ESP_BLE_MESH_MODEL_SCENE_CLI
 *
 *  @brief  Define a new Scene Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Scene Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Scene Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCENE_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCENE_CLI, \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set parameters structure.
 */

typedef struct {
    u16_t scene_number; /* The number of scenes to be stored */
} esp_ble_mesh_scene_store_t;

typedef struct {
    bool  op_en;        /* Indicate if optional parameters are included  */
    u16_t scene_number; /* The number of scenes to be recalled           */
    u8_t  tid;          /* Transaction ID                                */
    u8_t  trans_time;   /* Time to complete state transition (optional)  */
    u8_t  delay;        /* Indicate message execution delay (C.1)        */
} esp_ble_mesh_scene_recall_t;

typedef struct  {
    u16_t scene_number; /* The number of scenes to be deleted */
} esp_ble_mesh_scene_delete_t;

/**
 * @brief For
 *
 * the get_state parameter in the esp_ble_mesh_time_scene_client_get_state function should be set to NULL.
 */
typedef union {

} esp_ble_mesh_time_scene_client_get_state_t;

typedef union {
    esp_ble_mesh_scene_store_t  scene_store;       /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_STORE & ESP_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK */
    esp_ble_mesh_scene_recall_t scene_recall;      /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_RECALL & ESP_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK */
    esp_ble_mesh_scene_delete_t scene_delete;      /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_DELETE & ESP_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK */
} esp_ble_mesh_time_scene_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set callback parameters structure.
 */

typedef struct {
    bool  op_en;         /* Indicate if optional parameters are included  */
    u8_t  status_code;   /* Status code of the last operation             */
    u16_t current_scene; /* Scene Number of the current scene             */
    u16_t target_scene;  /* Scene Number of the target scene (optional)   */
    u8_t  remain_time;   /* Time to complete state transition (C.1)       */
} esp_ble_mesh_scene_status_cb_t;

typedef struct {
    u8_t  status_code;              /* Status code for the previous operation    */
    u16_t current_scene;            /* Scene Number of the current scene         */
    struct net_buf_simple *scenes;  /* A list of scenes stored within an element */
} esp_ble_mesh_scene_register_status_cb_t;



typedef union {
    esp_ble_mesh_scene_status_cb_t          scene_status;           /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_STATUS */
    esp_ble_mesh_scene_register_status_cb_t scene_register_status;  /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS */
} esp_ble_mesh_time_scene_client_status_cb_t;

typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t        *params;      /*!< The client common parameters. */
    esp_ble_mesh_time_scene_client_status_cb_t status_cb;   /*!< The scene status message callback values */
} esp_ble_mesh_time_scene_client_cb_param_t;

typedef enum {
    ESP_BLE_MESH_TIME_SCENE_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_TIME_SCENE_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_TIME_SCENE_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_TIME_SCENE_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_TIME_SCENE_CLIENT_EVT_MAX,
} esp_ble_mesh_time_scene_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Time Scene Client Model function.
 */

/** @brief: event, event code of Time Scene Client Model events; param, parameters of Time Scene Client Model events */
typedef void (* esp_mesh_time_scene_client_cb_t)(esp_ble_mesh_time_scene_client_cb_event_t event,
        esp_ble_mesh_time_scene_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Time Scene Client Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_time_scene_client_callback(esp_mesh_time_scene_client_cb_t callback);

/**
 * @brief       Get the value of Time Scene Server Model states using the Time Scene Client Model get messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to (@ref esp_ble_mesh_time_scene_message_opcode_t).
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   get_state: Pointer to time scene get message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 */
esp_err_t esp_ble_mesh_time_scene_client_get_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_time_scene_client_get_state_t *get_state);

/**
 * @brief       Set the value of Time Scene Server Model states using the Time Scene Client Model set messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to (@ref esp_ble_mesh_time_scene_message_opcode_t).
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer to time scene set message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 */
esp_err_t esp_ble_mesh_time_scene_client_set_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_time_scene_client_set_state_t *set_state);

#endif /* _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_ */

