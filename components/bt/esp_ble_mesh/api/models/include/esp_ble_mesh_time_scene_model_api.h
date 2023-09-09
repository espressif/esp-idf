/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Time and Scene Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_
#define _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

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

/** Parameters of Time Set */
typedef struct {
    uint8_t  tai_seconds[5];        /*!< The current TAI time in seconds */
    uint8_t  sub_second;            /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;           /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;    /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta : 15;    /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset;      /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_time_set_t;

/** Parameters of Time Zone Set */
typedef struct {
    uint8_t time_zone_offset_new;   /*!< Upcoming local time zone offset */
    uint8_t tai_zone_change[5];     /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_time_zone_set_t;

/** Parameters of TAI-UTC Delta Set */
typedef struct {
    uint16_t tai_utc_delta_new : 15;   /*!< Upcoming difference between TAI and UTC in seconds */
    uint16_t padding : 1;              /*!< Always 0b0. Other values are Prohibited. */
    uint8_t tai_delta_change[5];       /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_tai_utc_delta_set_t;

/** Parameter of Time Role Set */
typedef struct {
    uint8_t time_role;      /*!< The Time Role for the element */
} esp_ble_mesh_time_role_set_t;

/** Parameter of Scene Store */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be stored */
} esp_ble_mesh_scene_store_t;

/** Parameters of Scene Recall */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t scene_number;  /*!< The number of scenes to be recalled */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_scene_recall_t;

/** Parameter of Scene Delete */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be deleted */
} esp_ble_mesh_scene_delete_t;

/** Parameter of Scheduler Action Get */
typedef struct {
    uint8_t index;  /*!< Index of the Schedule Register entry to get */
} esp_ble_mesh_scheduler_act_get_t;

/** Parameters of Scheduler Action Set */
typedef struct {
    uint64_t index : 4;         /*!< Index of the Schedule Register entry to set */
    uint64_t year : 7;          /*!< Scheduled year for the action */
    uint64_t month : 12;        /*!< Scheduled month for the action */
    uint64_t day : 5;           /*!< Scheduled day of the month for the action */
    uint64_t hour : 5;          /*!< Scheduled hour for the action */
    uint64_t minute : 6;        /*!< Scheduled minute for the action */
    uint64_t second : 6;        /*!< Scheduled second for the action */
    uint64_t day_of_week : 7;   /*!< Schedule days of the week for the action */
    uint64_t action : 4;        /*!< Action to be performed at the scheduled time */
    uint64_t trans_time : 8;    /*!< Transition time for this action */
    uint16_t scene_number;      /*!< Transition time for this action */
} esp_ble_mesh_scheduler_act_set_t;

/**
 * @brief Time Scene Client Model get message union
 */
typedef union {
    esp_ble_mesh_scheduler_act_get_t scheduler_act_get; /*!< For ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET */
} esp_ble_mesh_time_scene_client_get_state_t;

/**
 * @brief Time Scene Client Model set message union
 */
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

/** Parameters of Time Status */
typedef struct {
    uint8_t  tai_seconds[5];           /*!< The current TAI time in seconds */
    uint8_t  sub_second;               /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;              /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;       /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta : 15;       /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset;         /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_time_status_cb_t;

/** Parameters of Time Zone Status */
typedef struct {
    uint8_t time_zone_offset_curr;     /*!< Current local time zone offset */
    uint8_t time_zone_offset_new;      /*!< Upcoming local time zone offset */
    uint8_t tai_zone_change[5];        /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_time_zone_status_cb_t;

/** Parameters of TAI-UTC Delta Status */
typedef struct {
    uint16_t tai_utc_delta_curr : 15;  /*!< Current difference between TAI and UTC in seconds */
    uint16_t padding_1 : 1;            /*!< Always 0b0. Other values are Prohibited. */
    uint16_t tai_utc_delta_new : 15;   /*!< Upcoming difference between TAI and UTC in seconds */
    uint16_t padding_2 : 1;            /*!< Always 0b0. Other values are Prohibited. */
    uint8_t tai_delta_change[5];       /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_tai_utc_delta_status_cb_t;

/** Parameter of Time Role Status */
typedef struct {
    uint8_t time_role;  /*!< The Time Role for the element */
} esp_ble_mesh_time_role_status_cb_t;

/** Parameters of Scene Status */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint8_t  status_code;   /*!< Status code of the last operation */
    uint16_t current_scene; /*!< Scene Number of the current scene */
    uint16_t target_scene;  /*!< Scene Number of the target scene (optional) */
    uint8_t  remain_time;   /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_scene_status_cb_t;

/** Parameters of Scene Register Status */
typedef struct {
    uint8_t  status_code;   /*!< Status code for the previous operation */
    uint16_t current_scene; /*!< Scene Number of the current scene */
    struct net_buf_simple *scenes;  /*!< A list of scenes stored within an element */
} esp_ble_mesh_scene_register_status_cb_t;

/** Parameter of Scheduler Status */
typedef struct {
    uint16_t schedules;     /*!< Bit field indicating defined Actions in the Schedule Register */
} esp_ble_mesh_scheduler_status_cb_t;

/** Parameters of Scheduler Action Status */
typedef struct {
    uint64_t index : 4;         /*!< Enumerates (selects) a Schedule Register entry */
    uint64_t year : 7;          /*!< Scheduled year for the action */
    uint64_t month : 12;        /*!< Scheduled month for the action */
    uint64_t day : 5;           /*!< Scheduled day of the month for the action */
    uint64_t hour : 5;          /*!< Scheduled hour for the action */
    uint64_t minute : 6;        /*!< Scheduled minute for the action */
    uint64_t second : 6;        /*!< Scheduled second for the action */
    uint64_t day_of_week : 7;   /*!< Schedule days of the week for the action */
    uint64_t action : 4;        /*!< Action to be performed at the scheduled time */
    uint64_t trans_time : 8;    /*!< Transition time for this action */
    uint16_t scene_number;      /*!< Transition time for this action */
} esp_ble_mesh_scheduler_act_status_cb_t;

/**
 * @brief Time Scene Client Model received message union
 */
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

/** Time Scene Client Model callback parameters */
typedef struct {
    int error_code;                                         /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t        *params;      /*!< The client common parameters. */
    esp_ble_mesh_time_scene_client_status_cb_t status_cb;   /*!< The scene status message callback values */
} esp_ble_mesh_time_scene_client_cb_param_t;

/** This enum value is the event of Time Scene Client Model */
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

/**
 * @brief   Time Scene Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
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
 *              please refer to esp_ble_mesh_time_scene_message_opcode_t in esp_ble_mesh_defs.h
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
 *              please refer to esp_ble_mesh_time_scene_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer to time scene set message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 */
esp_err_t esp_ble_mesh_time_scene_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                                   esp_ble_mesh_time_scene_client_set_state_t *set_state);

/**
 * @brief Time Scene Server Models related context.
 */

/** @def    ESP_BLE_MESH_MODEL_TIME_SRV
 *
 *  @brief  Define a new Time Server Model.
 *
 *  @note   1. The Time Server model is a root model. When this model is present on an
 *             Element, the corresponding Time Setup Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_time_srv_t.
 *
 *  @return New Time Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_TIME_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_TIME_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_TIME_SETUP_SRV
 *
 *  @brief  Define a new Time Setup Server Model.
 *
 *  @note   1. The Time Setup Server model extends the Time Server model. Time is
 *             sensitive information that is propagated across a mesh network.
 *          2. Only an authorized Time Client should be allowed to change the Time
 *             and Time Role states. A dedicated application key Bluetooth SIG
 *             Proprietary should be used on the Time Setup Server to restrict
 *             access to the server to only authorized Time Clients.
 *          3. This model does not support subscribing nor publishing.
 *
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_time_setup_srv_t.
 *
 *  @return New Time Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_TIME_SETUP_SRV(srv_data)                     \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_TIME_SETUP_SRV,    \
                    NULL, NULL, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SCENE_SRV
 *
 *  @brief  Define a new Scene Server Model.
 *
 *  @note   1. The Scene Server model is a root model. When this model is present
 *             on an Element, the corresponding Scene Setup Server model shall
 *             also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. The model may be present only on the Primary element of a node.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_scene_srv_t.
 *
 *  @return New Scene Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCENE_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCENE_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SCENE_SETUP_SRV
 *
 *  @brief  Define a new Scene Setup Server Model.
 *
 *  @note   1. The Scene Setup Server model extends the Scene Server model and
 *             the Generic Default Transition Time Server model.
 *          2. This model shall support model subscription.
 *          3. The model may be present only on the Primary element of a node.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_scene_setup_srv_t.
 *
 *  @return New Scene Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCENE_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCENE_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SCHEDULER_SRV
 *
 *  @brief  Define a new Scheduler Server Model.
 *
 *  @note   1. The Scheduler Server model extends the Scene Server model. When
 *             this model is present on an Element, the corresponding Scheduler
 *             Setup Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. The model may be present only on the Primary element of a node.
 *          4. The model requires the Time Server model shall be present on the element.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_scheduler_srv_t.
 *
 *  @return New Scheduler Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCHEDULER_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCHEDULER_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SCHEDULER_SETUP_SRV
 *
 *  @brief  Define a new Scheduler Setup Server Model.
 *
 *  @note   1. The Scheduler Setup Server model extends the Scheduler Server and
 *             the Scene Setup Server models.
 *          2. This model shall support model subscription.
 *          3. The model may be present only on the Primary element of a node.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_scheduler_setup_srv_t.
 *
 *  @return New Scheduler Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SCHEDULER_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

#define ESP_BLE_MESH_UNKNOWN_TAI_SECONDS        0x0000000000    /*!< Unknown TAI Seconds */
#define ESP_BLE_MESH_UNKNOWN_TAI_ZONE_CHANGE    0x0000000000    /*!< Unknown TAI of Zone Change */
#define ESP_BLE_MESH_UNKNOWN_TAI_DELTA_CHANGE   0x0000000000    /*!< Unknown TAI of Delta Change */

#define ESP_BLE_MESH_TAI_UTC_DELTA_MAX_VALUE    0x7FFF  /*!< Maximum TAI-UTC Delta value */

#define ESP_BLE_MESH_TAI_SECONDS_LEN            0x05    /*!< Length of TAI Seconds */
#define ESP_BLE_MESH_TAI_OF_ZONE_CHANGE_LEN     0x05    /*!< Length of TAI of Zone Change */
#define ESP_BLE_MESH_TAI_OF_DELTA_CHANGE_LEN    0x05    /*!< Length of TAI of Delta Change */

#define ESP_BLE_MESH_INVALID_SCENE_NUMBER       0x0000  /*!< Invalid Scene Number */
#define ESP_BLE_MESH_SCENE_NUMBER_LEN           0x02    /*!< Length of the Scene Number */

#define ESP_BLE_MESH_SCHEDULE_YEAR_ANY_YEAR     0x64    /*!< Any year of the Scheduled year */

#define ESP_BLE_MESH_SCHEDULE_DAY_ANY_DAY       0x00    /*!< Any day of the Scheduled day */

#define ESP_BLE_MESH_SCHEDULE_HOUR_ANY_HOUR     0x18    /*!< Any hour of the Scheduled hour */
#define ESP_BLE_MESH_SCHEDULE_HOUR_ONCE_A_DAY   0x19    /*!< Any hour of the Scheduled Day */

#define ESP_BLE_MESH_SCHEDULE_SEC_ANY_OF_HOUR   0x3C    /*!< Any minute of the Scheduled hour */
#define ESP_BLE_MESH_SCHEDULE_SEC_EVERY_15_MIN  0x3D    /*!< Every 15 minutes of the Scheduled hour */
#define ESP_BLE_MESH_SCHEDULE_SEC_EVERY_20_MIN  0x3E    /*!< Every 20 minutes of the Scheduled hour */
#define ESP_BLE_MESH_SCHEDULE_SEC_ONCE_AN_HOUR  0x3F    /*!< Once of the Scheduled hour */

#define ESP_BLE_MESH_SCHEDULE_SEC_ANY_OF_MIN    0x3C    /*!< Any second of the Scheduled minute */
#define ESP_BLE_MESH_SCHEDULE_SEC_EVERY_15_SEC  0x3D    /*!< Every 15 seconds of the Scheduled minute */
#define ESP_BLE_MESH_SCHEDULE_SEC_EVERY_20_SEC  0x3E    /*!< Every 20 seconds of the Scheduled minute */
#define ESP_BLE_MESH_SCHEDULE_SEC_ONCE_AN_MIN   0x3F    /*!< Once of the Scheduled minute */

#define ESP_BLE_MESH_SCHEDULE_ACT_TURN_OFF      0x00    /*!< Scheduled Action - Turn Off */
#define ESP_BLE_MESH_SCHEDULE_ACT_TURN_ON       0x01    /*!< Scheduled Action - Turn On */
#define ESP_BLE_MESH_SCHEDULE_ACT_SCENE_RECALL  0x02    /*!< Scheduled Action - Scene Recall */
#define ESP_BLE_MESH_SCHEDULE_ACT_NO_ACTION     0x0F    /*!< Scheduled Action - No Action */

#define ESP_BLE_MESH_SCHEDULE_SCENE_NO_SCENE    0x0000  /*!< Scheduled Scene - No Scene */

#define ESP_BLE_MESH_SCHEDULE_ENTRY_MAX_INDEX   0x0F    /*!< Maximum number of Scheduled entries */

#define ESP_BLE_MESH_TIME_NONE          0x00    /*!< Time Role - None */
#define ESP_BLE_MESH_TIME_AUTHORITY     0x01    /*!< Time Role - Mesh Time Authority */
#define ESP_BLE_MESH_TIME_RELAY         0x02    /*!< Time Role - Mesh Time Relay */
#define ESP_BLE_MESH_TIME_CLIENT        0x03    /*!< Time Role - Mesh Time Client */

#define ESP_BLE_MESH_SCENE_SUCCESS      0x00    /*!< Scene operation - Success */
#define ESP_BLE_MESH_SCENE_REG_FULL     0x01    /*!< Scene operation - Scene Register Full */
#define ESP_BLE_MESH_SCENE_NOT_FOUND    0x02    /*!< Scene operation - Scene Not Found */

/** Parameters of Time state */
typedef struct {
    struct {
        uint8_t  tai_seconds[5];            /*!< The value of the TAI Seconds state */
        uint8_t  subsecond;                 /*!< The value of the Subsecond field */
        uint8_t  uncertainty;               /*!< The value of the Uncertainty field */
        uint8_t  time_zone_offset_curr;     /*!< The value of the Time Zone Offset Current field */
        uint8_t  time_zone_offset_new;      /*!< The value of the Time Zone Offset New state */
        uint8_t  tai_zone_change[5];        /*!< The value of the TAI of Zone Change field */
        uint16_t time_authority : 1,        /*!< The value of the Time Authority bit */
                 tai_utc_delta_curr : 15;   /*!< The value of the TAI-UTC Delta Current state */
        uint16_t tai_utc_delta_new : 15;    /*!< The value of the TAI-UTC Delta New state */
        uint8_t  tai_delta_change[5];       /*!< The value of the TAI of Delta Change field */
    } time;                                 /*!< Parameters of the Time state */
    uint8_t time_role;                      /*!< The value of the Time Role state */
} esp_ble_mesh_time_state_t;

/** User data of Time Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Time Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_time_state_t *state;           /*!< Parameters of the Time state */
} esp_ble_mesh_time_srv_t;

/** User data of Time Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Time Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_time_state_t *state;           /*!< Parameters of the Time state */
} esp_ble_mesh_time_setup_srv_t;

/**
 * 1. Scene Store is an operation of storing values of a present state of an element.
 * 2. The structure and meaning of the stored state is determined by a model. States
 *    to be stored are specified by each model.
 * 3. The Scene Store operation shall persistently store all values of all states
 *    marked as Stored with Scene for all models present on all elements of a node.
 * 4. If a model is extending another model, the extending model shall determine the
 *    Stored with Scene behavior of that model.
 */

/** Parameters of Scene Register state */
typedef struct {
    uint16_t scene_number;  /*!< The value of the Scene Number */
    uint8_t  scene_type;    /*!< The value of the Scene Type */
    /**
     * Scene value may use a union to represent later, the union contains
     * structures of all the model states which can be stored in a scene.
     */
    struct net_buf_simple *scene_value; /*!< The value of the Scene Value */
} esp_ble_mesh_scene_register_t;

/**
 * Parameters of Scenes state.
 *
 * Scenes serve as memory banks for storage of states (e.g., a power level
 * or a light level/color). Values of states of an element can be stored
 * as a scene and can be recalled later from the scene memory.
 *
 * A scene is represented by a Scene Number, which is a 16-bit non-zero,
 * mesh-wide value. (There can be a maximum of 65535 scenes in a mesh
 * network.) The meaning of a scene, as well as the state storage container
 * associated with it, are determined by a model.
 *
 * The Scenes state change may start numerous parallel model transitions.
 * In that case, each individual model handles the transition internally.
 *
 * The scene transition is defined as a group of individual model transitions
 * started by a Scene Recall operation. The scene transition is in progress
 * when at least one transition from the group of individual model transitions
 * is in progress.
 */
typedef struct {
    const uint16_t scene_count;             /*!< The Scenes state's scene count */
    esp_ble_mesh_scene_register_t *scenes;  /*!< Parameters of the Scenes state */

    /**
     * The Current Scene state is a 16-bit value that contains either the Scene
     * Number of the currently active scene or a value of 0x0000 when no scene
     * is active.
     *
     * When a Scene Store operation or a Scene Recall operation completes with
     * success, the Current Scene state value shall be to the Scene Number used
     * during that operation.
     *
     * When the Current Scene Number is deleted from a Scene Register state as a
     * result of Scene Delete operation, the Current Scene state shall be set to
     * 0x0000.
     *
     * When any of the element's state that is marked as “Stored with Scene” has
     * changed not as a result of a Scene Recall operation, the value of the
     * Current Scene state shall be set to 0x0000.
     *
     * When a scene transition is in progress, the value of the Current Scene
     * state shall be set to 0x0000.
     */
    uint16_t current_scene;     /*!< The value of the Current Scene state */

    /**
     * The Target Scene state is a 16-bit value that contains the target Scene
     * Number when a scene transition is in progress.
     *
     * When the scene transition is in progress and the target Scene Number is
     * deleted from a Scene Register state as a result of Scene Delete operation,
     * the Target Scene state shall be set to 0x0000.
     *
     * When the scene transition is in progress and a new Scene Number is stored
     * in the Scene Register as a result of Scene Store operation, the Target
     * Scene state shall be set to the new Scene Number.
     *
     * When the scene transition is not in progress, the value of the Target Scene
     * state shall be set to 0x0000.
     */
    uint16_t target_scene;      /*!< The value of the Target Scene state */

    /* Indicate the status code for the last operation */
    uint8_t status_code;        /*!< The status code of the last scene operation */

    /* Indicate if scene transition is in progress */
    bool in_progress;           /*!< Indicate if the scene transition is in progress */
} esp_ble_mesh_scenes_state_t;

/** User data of Scene Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Scene Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_scenes_state_t *state;         /*!< Parameters of the Scenes state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
} esp_ble_mesh_scene_srv_t;

/** User data of Scene Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Scene Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_scenes_state_t *state;         /*!< Parameters of the Scenes state */
} esp_ble_mesh_scene_setup_srv_t;

/** Parameters of Scheduler Register state */
typedef struct {
    bool in_use;                /*!< Indicate if the registered schedule is in use */
    uint64_t year : 7,          /*!< The value of Scheduled year for the action */
             month : 12,        /*!< The value of Scheduled month for the action */
             day : 5,           /*!< The value of Scheduled day of the month for the action */
             hour : 5,          /*!< The value of Scheduled hour for the action */
             minute : 6,        /*!< The value of Scheduled minute for the action */
             second : 6,        /*!< The value of Scheduled second for the action */
             day_of_week : 7,   /*!< The value of Schedule days of the week for the action */
             action : 4,        /*!< The value of Action to be performed at the scheduled time */
             trans_time : 8;    /*!< The value of Transition time for this action */
    uint16_t scene_number;      /*!< The value of Scene Number to be used for some actions */
} esp_ble_mesh_schedule_register_t;

/** Parameters of Scheduler state */
typedef struct {
    const uint8_t schedule_count;                   /*!< Scheduler count */
    esp_ble_mesh_schedule_register_t *schedules;    /*!< Up to 16 scheduled entries */
} esp_ble_mesh_scheduler_state_t;

/** User data of Scheduler Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Scheduler Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_scheduler_state_t *state;      /*!< Parameters of the Scheduler state */
} esp_ble_mesh_scheduler_srv_t;

/** User data of Scheduler Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Scheduler Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_scheduler_state_t *state;      /*!< Parameters of the Scheduler state */
} esp_ble_mesh_scheduler_setup_srv_t;

/** Parameters of Time Set state change event */
typedef struct {
    uint8_t  tai_seconds[5];        /*!< The current TAI time in seconds */
    uint8_t  subsecond;             /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;           /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;    /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta_curr : 15;   /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset_curr;     /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_state_change_time_set_t;

/** Parameters of Time Status state change event */
typedef struct {
    uint8_t  tai_seconds[5];        /*!< The current TAI time in seconds */
    uint8_t  subsecond;             /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;           /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;    /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta_curr : 15;   /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset_curr;     /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_state_change_time_status_t;

/** Parameters of Time Zone Set state change event */
typedef struct {
    uint8_t time_zone_offset_new;   /*!< Upcoming local time zone offset */
    uint8_t tai_zone_change[5];     /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_state_change_time_zone_set_t;

/** Parameters of TAI UTC Delta Set state change event */
typedef struct {
    uint16_t tai_utc_delta_new : 15;    /*!< Upcoming difference between TAI and UTC in seconds */
    uint8_t tai_delta_change[5];        /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_state_change_tai_utc_delta_set_t;

/** Parameter of Time Role Set state change event */
typedef struct {
    uint8_t time_role;      /*!< The Time Role for the element */
} esp_ble_mesh_state_change_time_role_set_t;

/** Parameter of Scene Store state change event */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be stored */
} esp_ble_mesh_state_change_scene_store_t;

/** Parameter of Scene Recall state change event */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be recalled */
} esp_ble_mesh_state_change_scene_recall_t;

/** Parameter of Scene Delete state change event */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be deleted */
} esp_ble_mesh_state_change_scene_delete_t;

/** Parameter of Scheduler Action Set state change event */
typedef struct {
    uint64_t index : 4;         /*!< Index of the Schedule Register entry to set */
    uint64_t year : 7;          /*!< Scheduled year for the action */
    uint64_t month : 12;        /*!< Scheduled month for the action */
    uint64_t day : 5;           /*!< Scheduled day of the month for the action */
    uint64_t hour : 5;          /*!< Scheduled hour for the action */
    uint64_t minute : 6;        /*!< Scheduled minute for the action */
    uint64_t second : 6;        /*!< Scheduled second for the action */
    uint64_t day_of_week : 7;   /*!< Schedule days of the week for the action */
    uint64_t action : 4;        /*!< Action to be performed at the scheduled time */
    uint64_t trans_time : 8;    /*!< Transition time for this action */
    uint16_t scene_number;      /*!< Scene number to be used for some actions */
} esp_ble_mesh_state_change_scheduler_act_set_t;

/**
 * @brief Time Scene Server Model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_time_set_t          time_set;             /*!< Time Set */
    esp_ble_mesh_state_change_time_status_t       time_status;          /*!< Time Status */
    esp_ble_mesh_state_change_time_zone_set_t     time_zone_set;        /*!< Time Zone Set */
    esp_ble_mesh_state_change_tai_utc_delta_set_t tai_utc_delta_set;    /*!< TAI UTC Delta Set */
    esp_ble_mesh_state_change_time_role_set_t     time_role_set;        /*!< Time Role Set */
    esp_ble_mesh_state_change_scene_store_t       scene_store;          /*!< Scene Store */
    esp_ble_mesh_state_change_scene_recall_t      scene_recall;         /*!< Scene Recall */
    esp_ble_mesh_state_change_scene_delete_t      scene_delete;         /*!< Scene Delete */
    esp_ble_mesh_state_change_scheduler_act_set_t scheduler_act_set;    /*!< Scheduler Action Set */
} esp_ble_mesh_time_scene_server_state_change_t;

/** Context of the received Scheduler Action Get message */
typedef struct {
    uint8_t index;  /*!< Index of the Schedule Register entry to get */
} esp_ble_mesh_server_recv_scheduler_act_get_t;

/**
 * @brief Time Scene Server Model received get message union
 */
typedef union {
    esp_ble_mesh_server_recv_scheduler_act_get_t scheduler_act; /*!< Scheduler Action Get */
} esp_ble_mesh_time_scene_server_recv_get_msg_t;

/** Context of the received Time Set message */
typedef struct {
    uint8_t  tai_seconds[5];        /*!< The current TAI time in seconds */
    uint8_t  subsecond;             /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;           /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;    /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta : 15;    /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset;      /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_server_recv_time_set_t;

/** Context of the received Time Zone Set message */
typedef struct {
    uint8_t time_zone_offset_new;   /*!< Upcoming local time zone offset */
    uint8_t tai_zone_change[5];     /*!< TAI Seconds time of the upcoming Time Zone Offset change */
} esp_ble_mesh_server_recv_time_zone_set_t;

/** Context of the received TAI UTC Delta Set message */
typedef struct {
    uint16_t tai_utc_delta_new : 15;    /*!< Upcoming difference between TAI and UTC in seconds */
    uint16_t padding : 1;               /*!< Always 0b0. Other values are Prohibited. */
    uint8_t tai_delta_change[5];        /*!< TAI Seconds time of the upcoming TAI-UTC Delta change */
} esp_ble_mesh_server_recv_tai_utc_delta_set_t;

/** Context of the received Time Role Set message */
typedef struct {
    uint8_t time_role;      /*!< The Time Role for the element */
} esp_ble_mesh_server_recv_time_role_set_t;

/** Context of the received Scene Store message */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be stored */
} esp_ble_mesh_server_recv_scene_store_t;

/** Context of the received Scene Recall message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t scene_number;  /*!< The number of scenes to be recalled */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_scene_recall_t;

/** Context of the received Scene Delete message */
typedef struct {
    uint16_t scene_number;  /*!< The number of scenes to be deleted */
} esp_ble_mesh_server_recv_scene_delete_t;

/** Context of the received Scheduler Action Set message */
typedef struct {
    uint64_t index : 4;         /*!< Index of the Schedule Register entry to set */
    uint64_t year : 7;          /*!< Scheduled year for the action */
    uint64_t month : 12;        /*!< Scheduled month for the action */
    uint64_t day : 5;           /*!< Scheduled day of the month for the action */
    uint64_t hour : 5;          /*!< Scheduled hour for the action */
    uint64_t minute : 6;        /*!< Scheduled minute for the action */
    uint64_t second : 6;        /*!< Scheduled second for the action */
    uint64_t day_of_week : 7;   /*!< Schedule days of the week for the action */
    uint64_t action : 4;        /*!< Action to be performed at the scheduled time */
    uint64_t trans_time : 8;    /*!< Transition time for this action */
    uint16_t scene_number;      /*!< Scene number to be used for some actions */
} esp_ble_mesh_server_recv_scheduler_act_set_t;

/**
 * @brief Time Scene Server Model received set message union
 */
typedef union {
    esp_ble_mesh_server_recv_time_set_t          time;              /*!< Time Set */
    esp_ble_mesh_server_recv_time_zone_set_t     time_zone;         /*!< Time Zone Set */
    esp_ble_mesh_server_recv_tai_utc_delta_set_t tai_utc_delta;     /*!< TAI-UTC Delta Set */
    esp_ble_mesh_server_recv_time_role_set_t     time_role;         /*!< Time Role Set */
    esp_ble_mesh_server_recv_scene_store_t       scene_store;       /*!< Scene Store/Scene Store Unack */
    esp_ble_mesh_server_recv_scene_recall_t      scene_recall;      /*!< Scene Recall/Scene Recall Unack */
    esp_ble_mesh_server_recv_scene_delete_t      scene_delete;      /*!< Scene Delete/Scene Delete Unack */
    esp_ble_mesh_server_recv_scheduler_act_set_t scheduler_act;     /*!< Scheduler Action Set/Scheduler Action Set Unack */
} esp_ble_mesh_time_scene_server_recv_set_msg_t;

/** Context of the received Time Status message */
typedef struct {
    uint8_t  tai_seconds[5];        /*!< The current TAI time in seconds */
    uint8_t  subsecond;             /*!< The sub-second time in units of 1/256 second */
    uint8_t  uncertainty;           /*!< The estimated uncertainty in 10-millisecond steps */
    uint16_t time_authority : 1;    /*!< 0 = No Time Authority, 1 = Time Authority */
    uint16_t tai_utc_delta : 15;    /*!< Current difference between TAI and UTC in seconds */
    uint8_t  time_zone_offset;      /*!< The local time zone offset in 15-minute increments */
} esp_ble_mesh_server_recv_time_status_t;

/**
 * @brief Time Scene Server Model received status message union
 */
typedef union {
    esp_ble_mesh_server_recv_time_status_t time_status;     /*!< Time Status */
} esp_ble_mesh_time_scene_server_recv_status_msg_t;

/**
 * @brief Time Scene Server Model callback value union
 */
typedef union {
    esp_ble_mesh_time_scene_server_state_change_t state_change; /*!< ESP_BLE_MESH_TIME_SCENE_SERVER_STATE_CHANGE_EVT */
    esp_ble_mesh_time_scene_server_recv_get_msg_t get;  /*!< ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_GET_MSG_EVT */
    esp_ble_mesh_time_scene_server_recv_set_msg_t set;  /*!< ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_SET_MSG_EVT */
    esp_ble_mesh_time_scene_server_recv_status_msg_t status;    /*!< ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_STATUS_MSG_EVT */
} esp_ble_mesh_time_scene_server_cb_value_t;

/** Time Scene Server Model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;   /*!< Pointer to Time and Scenes Server Models */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received messages */
    esp_ble_mesh_time_scene_server_cb_value_t value;    /*!< Value of the received Time and Scenes Messages */
} esp_ble_mesh_time_scene_server_cb_param_t;

/** This enum value is the event of Time Scene Server Model */
typedef enum {
    /**
     * 1. When get_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, no event will be
     *    callback to the application layer when Time Scene Get messages are received.
     * 2. When set_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, this event will
     *    be callback to the application layer when Time Scene Set/Set Unack messages
     *    are received.
     */
    ESP_BLE_MESH_TIME_SCENE_SERVER_STATE_CHANGE_EVT,
    /**
     * When get_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Time Scene Get messages are received.
     */
    ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_GET_MSG_EVT,
    /**
     * When set_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Time Scene Set/Set Unack messages are received.
     */
    ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_SET_MSG_EVT,
    /**
     * When status_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will
     * be callback to the application layer when TIme Status message is received.
     */
    ESP_BLE_MESH_TIME_SCENE_SERVER_RECV_STATUS_MSG_EVT,
    ESP_BLE_MESH_TIME_SCENE_SERVER_EVT_MAX,
} esp_ble_mesh_time_scene_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Time and Scenes Server Model function.
 */

/**
 * @brief   Time Scene Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_time_scene_server_cb_t)(esp_ble_mesh_time_scene_server_cb_event_t event,
                                                     esp_ble_mesh_time_scene_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Time and Scenes Server Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_time_scene_server_callback(esp_ble_mesh_time_scene_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_TIME_SCENE_MODEL_API_H_ */
