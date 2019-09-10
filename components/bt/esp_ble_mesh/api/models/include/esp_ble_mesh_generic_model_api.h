// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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
 *  @brief Bluetooth Mesh Generic Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_GENERIC_MODEL_API_H_
#define _ESP_BLE_MESH_GENERIC_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

/** @def    ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI
 *
 *  @brief  Define a new Generic OnOff Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic OnOff Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic OnOff Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_ONOFF_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_LEVEL_CLI
 *
 *  @brief  Define a new Generic Level Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Level Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Level Client Model instance.
 */

#define ESP_BLE_MESH_MODEL_GEN_LEVEL_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI, \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI
 *
 *  @brief  Define a new Generic Default Transition Time Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Default Transition
 *          Time Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Default Transition Time Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI(cli_pub, cli_data)            \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI,    \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI
 *
 *  @brief  Define a new Generic Power OnOff Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Power OnOff Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Power OnOff Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI(cli_pub, cli_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI,   \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI
 *
 *  @brief  Define a new Generic Power Level Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Power Level Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Power Level Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI(cli_pub, cli_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI,   \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_BATTERY_CLI
 *
 *  @brief  Define a new Generic Battery Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Battery Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Battery Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_BATTERY_CLI(cli_pub, cli_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_CLI,   \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_LOCATION_CLI
 *
 *  @brief  Define a new Generic Location Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Location Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Location Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_LOCATION_CLI(cli_pub, cli_data)          \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI,  \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_PROPERTY_CLI
 *
 *  @brief  Define a new Generic Property Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Generic Property Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Generic Location Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_PROPERTY_CLI(cli_pub, cli_data)          \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_PROP_CLI,      \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Generic Client Model Get and Set parameters structure.
 */

/** Parameters of Generic OnOff Set. */
typedef struct {
    bool    op_en;      /*!< Indicate if optional parameters are included */
    uint8_t onoff;      /*!< Target value of Generic OnOff state */
    uint8_t tid;        /*!< Transaction ID */
    uint8_t trans_time; /*!< Time to complete state transition (optional) */
    uint8_t delay;      /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_gen_onoff_set_t;

/** Parameters of Generic Level Set. */
typedef struct {
    bool    op_en;      /*!< Indicate if optional parameters are included */
    int16_t level;      /*!< Target value of Generic Level state */
    uint8_t tid;        /*!< Transaction ID */
    uint8_t trans_time; /*!< Time to complete state transition (optional) */
    uint8_t delay;      /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_gen_level_set_t;

/** Parameters of Generic Delta Set. */
typedef struct {
    bool    op_en;      /*!< Indicate if optional parameters are included */
    int32_t level;      /*!< Delta change of Generic Level state */
    uint8_t tid;        /*!< Transaction ID */
    uint8_t trans_time; /*!< Time to complete state transition (optional) */
    uint8_t delay;      /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_gen_delta_set_t;

/** Parameters of Generic Move Set. */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    int16_t delta_level;    /*!< Delta Level step to calculate Move speed for Generic Level state */
    uint8_t tid;            /*!< Transaction ID */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_gen_move_set_t;

/** Parameter of Generic Default Transition Time Set. */
typedef struct {
    uint8_t trans_time; /*!< The value of the Generic Default Transition Time state */
} esp_ble_mesh_gen_def_trans_time_set_t;

/** Parameter of Generic OnPowerUp Set. */
typedef struct {
    uint8_t onpowerup;  /*!< The value of the Generic OnPowerUp state */
} esp_ble_mesh_gen_onpowerup_set_t;

/** Parameters of Generic Power Level Set. */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t power;         /*!< Target value of Generic Power Actual state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_gen_power_level_set_t;

/** Parameter of Generic Power Default Set. */
typedef struct {
    uint16_t power;         /*!< The value of the Generic Power Default state */
} esp_ble_mesh_gen_power_default_set_t;

/** Parameters of Generic Power Range Set. */
typedef struct {
    uint16_t range_min;     /*!< Value of Range Min field of Generic Power Range state */
    uint16_t range_max;     /*!< Value of Range Max field of Generic Power Range state */
} esp_ble_mesh_gen_power_range_set_t;

/** Parameters of Generic Location Global Set. */
typedef struct {
    int32_t global_latitude;    /*!< Global Coordinates (Latitude) */
    int32_t global_longitude;   /*!< Global Coordinates (Longitude) */
    int16_t global_altitude;    /*!< Global Altitude */
} esp_ble_mesh_gen_loc_global_set_t;

/** Parameters of Generic Location Local Set. */
typedef struct {
    int16_t  local_north;       /*!< Local Coordinates (North) */
    int16_t  local_east;        /*!< Local Coordinates (East) */
    int16_t  local_altitude;    /*!< Local Altitude */
    uint8_t  floor_number;      /*!< Floor Number */
    uint16_t uncertainty;       /*!< Uncertainty */
} esp_ble_mesh_gen_loc_local_set_t;

/** Parameter of Generic User Property Get. */
typedef struct {
    uint16_t property_id;       /*!< Property ID identifying a Generic User Property */
} esp_ble_mesh_gen_user_property_get_t;

/** Parameters of Generic User Property Set. */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic User Property */
    struct net_buf_simple *property_value;  /*!< Raw value for the User Property */
} esp_ble_mesh_gen_user_property_set_t;

/** Parameter of Generic Admin Property Get. */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Admin Property */
} esp_ble_mesh_gen_admin_property_get_t;

/** Parameters of Generic Admin Property Set. */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Admin Property */
    uint8_t  user_access;   /*!< Enumeration indicating user accessn */
    struct net_buf_simple *property_value;  /*!< Raw value for the Admin Property */
} esp_ble_mesh_gen_admin_property_set_t;

/** Parameter of Generic Manufacturer Property Get. */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Manufacturer Property */
} esp_ble_mesh_gen_manufacturer_property_get_t;

/** Parameters of Generic Manufacturer Property Set. */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Manufacturer Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access */
} esp_ble_mesh_gen_manufacturer_property_set_t;

/** Parameter of Generic Client Properties Get. */
typedef struct {
    uint16_t property_id;   /*!< A starting Client Property ID present within an element */
} esp_ble_mesh_gen_client_properties_get_t;

/**
 * @brief Generic Client Model get message union
 */
typedef union {
    esp_ble_mesh_gen_user_property_get_t         user_property_get;         /*!< For ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET */
    esp_ble_mesh_gen_admin_property_get_t        admin_property_get;        /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET*/
    esp_ble_mesh_gen_manufacturer_property_get_t manufacturer_property_get; /*!< For ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET */
    esp_ble_mesh_gen_client_properties_get_t     client_properties_get;     /*!< For ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET */
} esp_ble_mesh_generic_client_get_state_t;

/**
 * @brief Generic Client Model set message union
 */
typedef union {
    esp_ble_mesh_gen_onoff_set_t          onoff_set;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET & ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK */
    esp_ble_mesh_gen_level_set_t          level_set;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET & ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK */
    esp_ble_mesh_gen_delta_set_t          delta_set;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET & ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK */
    esp_ble_mesh_gen_move_set_t           move_set;             /*!< For ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET & ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK */
    esp_ble_mesh_gen_def_trans_time_set_t def_trans_time_set;   /*!< For ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET & ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK */
    esp_ble_mesh_gen_onpowerup_set_t      power_set;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET & ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK */
    esp_ble_mesh_gen_power_level_set_t    power_level_set;      /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET & ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK */
    esp_ble_mesh_gen_power_default_set_t  power_default_set;    /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET & ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK */
    esp_ble_mesh_gen_power_range_set_t    power_range_set;      /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET & ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK */
    esp_ble_mesh_gen_loc_global_set_t     loc_global_set;       /*!< For ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET & ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK */
    esp_ble_mesh_gen_loc_local_set_t      loc_local_set;        /*!< For ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET & ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK */
    esp_ble_mesh_gen_user_property_set_t  user_property_set;    /*!< For ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET & ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK */
    esp_ble_mesh_gen_admin_property_set_t admin_property_set;   /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET & ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK */
    esp_ble_mesh_gen_manufacturer_property_set_t manufacturer_property_set; /*!< For ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET & ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK */
} esp_ble_mesh_generic_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Generic Client Model Get and Set callback parameters structure.
 */

/** Parameters of Generic OnOff Status. */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    uint8_t present_onoff;  /*!< Current value of Generic OnOff state */
    uint8_t target_onoff;   /*!< Target value of Generic OnOff state (optional) */
    uint8_t remain_time;    /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_gen_onoff_status_cb_t;

/** Parameters of Generic Level Status. */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    int16_t present_level;  /*!< Current value of Generic Level state */
    int16_t target_level;   /*!< Target value of the Generic Level state (optional) */
    uint8_t remain_time;    /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_gen_level_status_cb_t;

/** Parameter of Generic Default Transition Time Status. */
typedef struct {
    uint8_t trans_time;     /*!< The value of the Generic Default Transition Time state */
} esp_ble_mesh_gen_def_trans_time_status_cb_t;

/** Parameter of Generic OnPowerUp Status. */
typedef struct {
    uint8_t onpowerup;      /*!< The value of the Generic OnPowerUp state */
} esp_ble_mesh_gen_onpowerup_status_cb_t;

/** Parameters of Generic Power Level Status. */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t present_power; /*!< Current value of Generic Power Actual state */
    uint16_t target_power;  /*!< Target value of Generic Power Actual state (optional) */
    uint8_t  remain_time;   /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_gen_power_level_status_cb_t;

/** Parameter of Generic Power Last Status. */
typedef struct {
    uint16_t power;         /*!< The value of the Generic Power Last state */
} esp_ble_mesh_gen_power_last_status_cb_t;

/** Parameter of Generic Power Default Status. */
typedef struct {
    uint16_t power;         /*!< The value of the Generic Default Last state */
} esp_ble_mesh_gen_power_default_status_cb_t;

/** Parameters of Generic Power Range Status. */
typedef struct {
    uint8_t  status_code;   /*!< Status Code for the request message */
    uint16_t range_min;     /*!< Value of Range Min field of Generic Power Range state */
    uint16_t range_max;     /*!< Value of Range Max field of Generic Power Range state */
} esp_ble_mesh_gen_power_range_status_cb_t;

/** Parameters of Generic Battery Status. */
typedef struct {
    u32_t battery_level     : 8;  /*!< Value of Generic Battery Level state */
    u32_t time_to_discharge : 24; /*!< Value of Generic Battery Time to Discharge state */
    u32_t time_to_charge    : 24; /*!< Value of Generic Battery Time to Charge state */
    u32_t flags             : 8;  /*!< Value of Generic Battery Flags state */
} esp_ble_mesh_gen_battery_status_cb_t;

/** Parameters of Generic Location Global Status. */
typedef struct {
    int32_t global_latitude;  /*!< Global Coordinates (Latitude) */
    int32_t global_longitude; /*!< Global Coordinates (Longitude) */
    int16_t global_altitude;  /*!< Global Altitude */
} esp_ble_mesh_gen_loc_global_status_cb_t;

/** Parameters of Generic Location Local Status. */
typedef struct {
    int16_t  local_north;     /*!< Local Coordinates (North) */
    int16_t  local_east;      /*!< Local Coordinates (East) */
    int16_t  local_altitude;  /*!< Local Altitude */
    uint8_t  floor_number;    /*!< Floor Number */
    uint16_t uncertainty;     /*!< Uncertainty */
} esp_ble_mesh_gen_loc_local_status_cb_t;

/** Parameter of Generic User Properties Status. */
typedef struct {
    struct net_buf_simple *property_ids;    /*!< Buffer contains a sequence of N User Property IDs */
} esp_ble_mesh_gen_user_properties_status_cb_t;

/** Parameters of Generic User Property Status. */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t property_id;   /*!< Property ID identifying a Generic User Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access (optional) */
    struct net_buf_simple *property_value;  /*!< Raw value for the User Property (C.1) */
} esp_ble_mesh_gen_user_property_status_cb_t;

/** Parameter of Generic Admin Properties Status. */
typedef struct {
    struct net_buf_simple *property_ids; /*!< Buffer contains a sequence of N Admin Property IDs */
} esp_ble_mesh_gen_admin_properties_status_cb_t;

/** Parameters of Generic Admin Property Status. */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t property_id;   /*!< Property ID identifying a Generic Admin Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access (optional) */
    struct net_buf_simple *property_value;  /*!< Raw value for the Admin Property (C.1) */
} esp_ble_mesh_gen_admin_property_status_cb_t;

/** Parameter of Generic Manufacturer Properties Status. */
typedef struct {
    struct net_buf_simple *property_ids;    /*!< Buffer contains a sequence of N Manufacturer Property IDs */
} esp_ble_mesh_gen_manufacturer_properties_status_cb_t;

/** Parameters of Generic Manufacturer Property Status. */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t property_id;   /*!< Property ID identifying a Generic Manufacturer Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access (optional) */
    struct net_buf_simple *property_value;  /*!< Raw value for the Manufacturer Property (C.1) */
} esp_ble_mesh_gen_manufacturer_property_status_cb_t;

/** Parameter of Generic Client Properties Status. */
typedef struct {
    struct net_buf_simple *property_ids;    /*!< Buffer contains a sequence of N Client Property IDs */
} esp_ble_mesh_gen_client_properties_status_cb_t;

/**
 * @brief Generic Client Model received message union
 */
typedef union {
    esp_ble_mesh_gen_onoff_status_cb_t             onoff_status;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS */
    esp_ble_mesh_gen_level_status_cb_t             level_status;            /*!< For ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS */
    esp_ble_mesh_gen_def_trans_time_status_cb_t    def_trans_time_status;   /*!< For ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS */
    esp_ble_mesh_gen_onpowerup_status_cb_t         onpowerup_status;        /*!< For ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS */
    esp_ble_mesh_gen_power_level_status_cb_t       power_level_status;      /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS */
    esp_ble_mesh_gen_power_last_status_cb_t        power_last_status;       /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS */
    esp_ble_mesh_gen_power_default_status_cb_t     power_default_status;    /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS */
    esp_ble_mesh_gen_power_range_status_cb_t       power_range_status;      /*!< For ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS */
    esp_ble_mesh_gen_battery_status_cb_t           battery_status;          /*!< For ESP_BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS */
    esp_ble_mesh_gen_loc_global_status_cb_t        location_global_status;  /*!< For ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS */
    esp_ble_mesh_gen_loc_local_status_cb_t         location_local_status;   /*!< ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS */
    esp_ble_mesh_gen_user_properties_status_cb_t   user_properties_status;  /*!< ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS */
    esp_ble_mesh_gen_user_property_status_cb_t     user_property_status;    /*!< ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS */
    esp_ble_mesh_gen_admin_properties_status_cb_t  admin_properties_status; /*!< ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS */
    esp_ble_mesh_gen_admin_property_status_cb_t    admin_property_status;   /*!< ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS */
    esp_ble_mesh_gen_manufacturer_properties_status_cb_t manufacturer_properties_status; /*!< ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_STATUS */
    esp_ble_mesh_gen_manufacturer_property_status_cb_t   manufacturer_property_status;   /*!< ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_STATUS */
    esp_ble_mesh_gen_client_properties_status_cb_t       client_properties_status;       /*!< ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS */
} esp_ble_mesh_gen_client_status_cb_t;

/** Generic Client Model callback parameters */
typedef struct {
    int error_code;                                 /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t *params;     /*!< The client common parameters. */
    esp_ble_mesh_gen_client_status_cb_t status_cb;  /*!< The generic status message callback values */
} esp_ble_mesh_generic_client_cb_param_t;

/** This enum value is the event of Generic Client Model */
typedef enum {
    ESP_BLE_MESH_GENERIC_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_GENERIC_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_GENERIC_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_GENERIC_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_GENERIC_CLIENT_EVT_MAX,
} esp_ble_mesh_generic_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Generic Client Model function.
 */

/**
 * @brief   Generic Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_generic_client_cb_t)(esp_ble_mesh_generic_client_cb_event_t event,
        esp_ble_mesh_generic_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Generic Client Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_generic_client_callback(esp_ble_mesh_generic_client_cb_t callback);

/**
 * @brief       Get the value of Generic Server Model states using the Generic Client Model get messages.
 *
 * @note        If you want to find the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_generic_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   get_state: Pointer to generic get message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_generic_client_get_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_generic_client_get_state_t *get_state);

/**
 * @brief       Set the value of Generic Server Model states using the Generic Client Model set messages.
 *
 * @note        If you want to find the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_generic_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer to generic set message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_generic_client_set_state(esp_ble_mesh_client_common_param_t *params,
        esp_ble_mesh_generic_client_set_state_t *set_state);


#endif /* _ESP_BLE_MESH_GENERIC_MODEL_API_H_ */

