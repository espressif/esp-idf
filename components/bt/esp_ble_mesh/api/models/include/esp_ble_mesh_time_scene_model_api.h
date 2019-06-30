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

/** @def    ESP_BLE_MESH_MODEL_TIME_CLI
 *
 *  @brief  Define a new Time Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Time Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Time Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_TIME_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_TIME_CLI, \
                    NULL, cli_pub, cli_data)

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

/** @def    ESP_BLE_MESH_MODEL_SCHEDULER_CLI
 *
 *  @brief  Define a new Scheduler Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Scheduler Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Scheduler Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCHEDULER_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCHEDULER_CLI, \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set parameters structure.
 */

typedef struct {
    u8_t  tai_seconds[5];     /* The current TAI time in seconds */
    u8_t  sub_second;         /* The sub-second time in units of 1/256 second */
    u8_t  uncertainty;        /* The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1; /* 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15; /* Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;   /* The local time zone offset in 15-minute increments */
} esp_ble_mesh_time_set_t;

typedef struct {
    u8_t time_zone_offset_new; /* Upcoming local time zone offset */
    u8_t tai_zone_change[5];   /* TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_time_zone_set_t;

typedef struct {
    u16_t tai_utc_delta_new : 15; /* Upcoming difference between TAI and UTC in seconds */
    u16_t padding : 1;            /* Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];     /* TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_tai_utc_delta_set_t;

typedef struct {
    u8_t time_role; /* The Time Role for the element */
} esp_ble_mesh_time_role_set_t;

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

typedef struct {
    u16_t scene_number; /* The number of scenes to be deleted */
} esp_ble_mesh_scene_delete_t;

typedef struct {
    u8_t index; /* Index of the Schedule Register entry to get */
} esp_ble_mesh_scheduler_act_get_t;

typedef struct {
    u64_t index : 4;        /* Index of the Schedule Register entry to set */
    u64_t year : 7;         /* Scheduled year for the action */
    u64_t month : 12;       /* Scheduled month for the action */
    u64_t day : 5;          /* Scheduled day of the month for the action */
    u64_t hour : 5;         /* Scheduled hour for the action */
    u64_t minute : 6;       /* Scheduled minute for the action */
    u64_t second : 6;       /* Scheduled second for the action */
    u64_t day_of_week : 7;  /* Schedule days of the week for the action */
    u64_t action : 4;       /* Action to be performed at the scheduled time */
    u64_t trans_time : 8;   /* Transition time for this action */
    u16_t scene_number;     /* Transition time for this action */
} esp_ble_mesh_scheduler_act_set_t;

/**
 * @brief For
 *
 * the get_state parameter in the esp_ble_mesh_time_scene_client_get_state function should be set to NULL.
 */
typedef union {
    esp_ble_mesh_scheduler_act_get_t scheduler_act_get; /*!< For ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET */
} esp_ble_mesh_time_scene_client_get_state_t;

typedef union {
    esp_ble_mesh_time_set_t          time_set;          /*!< For ESP_BLE_MESH_MODEL_OP_TIME_SET */
    esp_ble_mesh_time_zone_set_t     time_zone_set;     /*!< For ESP_BLE_MESH_MODEL_OP_TIME_ZONE_SET */
    esp_ble_mesh_tai_utc_delta_set_t tai_utc_delta_set; /*!< For ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET */
    esp_ble_mesh_time_role_set_t     time_role_set;     /*!< For ESP_BLE_MESH_MODEL_OP_TIME_ROLE_SET */
    esp_ble_mesh_scene_store_t       scene_store;       /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_STORE & ESP_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK */
    esp_ble_mesh_scene_recall_t      scene_recall;      /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_RECALL & ESP_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK */
    esp_ble_mesh_scene_delete_t      scene_delete;      /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_DELETE & ESP_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK */
    esp_ble_mesh_scheduler_act_set_t scheduler_act_set; /*!< For ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET & ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK */
} esp_ble_mesh_time_scene_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Time Scene Client Model Get and Set callback parameters structure.
 */

typedef struct {
    u8_t  tai_seconds[5];     /* The current TAI time in seconds */
    u8_t  sub_second;         /* The sub-second time in units of 1/256 second */
    u8_t  uncertainty;        /* The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1; /* 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15; /* Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;   /* The local time zone offset in 15-minute increments */
} esp_ble_mesh_time_status_cb_t;

typedef struct {
    u8_t time_zone_offset_curr; /* Current local time zone offset */
    u8_t time_zone_offset_new;  /* Upcoming local time zone offset */
    u8_t tai_zone_change[5];    /* TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_time_zone_status_cb_t;

typedef struct {
    u16_t tai_utc_delta_curr : 15; /* Current difference between TAI and UTC in seconds */
    u16_t padding_1 : 1;           /* Always 0b0. Other values are Prohibited. */
    u16_t tai_utc_delta_new : 15;  /* Upcoming difference between TAI and UTC in seconds */
    u16_t padding_2 : 1;           /* Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];      /* TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_tai_utc_delta_status_cb_t;

typedef struct {
    u8_t time_role; /* The Time Role for the element */
} esp_ble_mesh_time_role_status_cb_t;

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

typedef struct {
    u16_t schedules; /* Bit field indicating defined Actions in the Schedule Register */
} esp_ble_mesh_scheduler_status_cb_t;

typedef struct {
    u64_t index : 4;        /* Enumerates (selects) a Schedule Register entry */
    u64_t year : 7;         /* Scheduled year for the action */
    u64_t month : 12;       /* Scheduled month for the action */
    u64_t day : 5;          /* Scheduled day of the month for the action */
    u64_t hour : 5;         /* Scheduled hour for the action */
    u64_t minute : 6;       /* Scheduled minute for the action */
    u64_t second : 6;       /* Scheduled second for the action */
    u64_t day_of_week : 7;  /* Schedule days of the week for the action */
    u64_t action : 4;       /* Action to be performed at the scheduled time */
    u64_t trans_time : 8;   /* Transition time for this action */
    u16_t scene_number;     /* Transition time for this action */
} esp_ble_mesh_scheduler_act_status_cb_t;

typedef union {
    esp_ble_mesh_time_status_cb_t           time_status;            /*!< For ESP_BLE_MESH_MODEL_OP_TIME_STATUS */
    esp_ble_mesh_time_zone_status_cb_t      time_zone_status;       /*!< For ESP_BLE_MESH_MODEL_OP_TIME_ZONE_STATUS */
    esp_ble_mesh_tai_utc_delta_status_cb_t  tai_utc_delta_status;   /*!< For ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS */
    esp_ble_mesh_time_role_status_cb_t      time_role_status;       /*!< For ESP_BLE_MESH_MODEL_OP_TIME_ROLE_STATUS */
    esp_ble_mesh_scene_status_cb_t          scene_status;           /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_STATUS */
    esp_ble_mesh_scene_register_status_cb_t scene_register_status;  /*!< For ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS */
    esp_ble_mesh_scheduler_status_cb_t      scheduler_status;       /*!< For ESP_BLE_MESH_MODEL_OP_SCHEDULER_STATUS */
    esp_ble_mesh_scheduler_act_status_cb_t  scheduler_act_status;   /*!< For ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS */
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
typedef void (* esp_ble_mesh_time_scene_client_cb_t)(esp_ble_mesh_time_scene_client_cb_event_t event,
        esp_ble_mesh_time_scene_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Time Scene Client Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_time_scene_client_callback(esp_ble_mesh_time_scene_client_cb_t callback);

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

