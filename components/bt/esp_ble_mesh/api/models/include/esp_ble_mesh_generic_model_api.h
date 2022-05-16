/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Generic Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_GENERIC_MODEL_API_H_
#define _ESP_BLE_MESH_GENERIC_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    uint8_t  user_access;   /*!< Enumeration indicating user access */
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
    uint32_t battery_level     : 8;  /*!< Value of Generic Battery Level state */
    uint32_t time_to_discharge : 24; /*!< Value of Generic Battery Time to Discharge state */
    uint32_t time_to_charge    : 24; /*!< Value of Generic Battery Time to Charge state */
    uint32_t flags             : 8;  /*!< Value of Generic Battery Flags state */
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

/**
 * @brief Generic Server Models related context.
 */

/** @def    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV
 *
 *  @brief  Define a new Generic OnOff Server Model.
 *
 *  @note   1. The Generic OnOff Server Model is a root model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_onoff_srv_t.
 *
 *  @return New Generic OnOff Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_LEVEL_SRV
 *
 *  @brief  Define a new Generic Level Server Model.
 *
 *  @note   1. The Generic Level Server Model is a root model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_level_srv_t.
 *
 *  @return New Generic Level Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_LEVEL_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV
 *
 *  @brief  Define a new Generic Default Transition Time Server Model.
 *
 *  @note   1. The Generic Default Transition Time Server Model is a root model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_def_trans_time_srv_t.
 *
 *  @return New Generic Default Transition Time Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_SRV
 *
 *  @brief  Define a new Generic Power OnOff Server Model.
 *
 *  @note   1. The Generic Power OnOff Server model extends the Generic OnOff Server
 *             model. When this model is present on an element, the corresponding
 *             Generic Power OnOff Setup Server model shall also be present.
 *          2. This model may be used to represent a variety of devices that do not
 *             fit any of the model descriptions that have been defined but support
 *             the generic properties of On/Off.
 *          3. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_power_onoff_srv_t.
 *
 *  @return New Generic Power OnOff Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_SETUP_SRV
 *
 *  @brief  Define a new Generic Power OnOff Setup Server Model.
 *
 *  @note   1. The Generic Power OnOff Setup Server model extends the Generic Power
 *             OnOff Server model and the Generic Default Transition Time Server model.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_power_onoff_setup_srv_t.
 *
 *  @return New Generic Power OnOff Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_ONOFF_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_SRV
 *
 *  @brief  Define a new Generic Power Level Server Model.
 *
 *  @note   1. The Generic Power Level Server model extends the Generic Power OnOff
 *             Server model and the Generic Level Server model. When this model is
 *             present on an Element, the corresponding Generic Power Level Setup
 *             Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_power_level_srv_t.
 *
 *  @return New Generic Power Level Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_SETUP_SRV
 *
 *  @brief  Define a new Generic Power Level Setup Server Model.
 *
 *  @note   1. The Generic Power Level Setup Server model extends the Generic Power
 *             Level Server model and the Generic Power OnOff Setup Server model.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_power_level_setup_srv_t.
 *
 *  @return New Generic Power Level Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_POWER_LEVEL_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_BATTERY_SRV
 *
 *  @brief  Define a new Generic Battery Server Model.
 *
 *  @note   1. The Generic Battery Server Model is a root model.
 *          2. This model shall support model publication and model subscription.
 *          3. The model may be used to represent an element that is powered by a battery.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_battery_srv_t.
 *
 *  @return New Generic Battery Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_BATTERY_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_LOCATION_SRV
 *
 *  @brief  Define a new Generic Location Server Model.
 *
 *  @note   1. The Generic Location Server model is a root model. When this model
 *             is present on an Element, the corresponding Generic Location Setup
 *             Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. The model may be used to represent an element that knows its
 *             location (global or local).
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_location_srv_t.
 *
 *  @return New Generic Location Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_LOCATION_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_LOCATION_SETUP_SRV
 *
 *  @brief  Define a new Generic Location Setup Server Model.
 *
 *  @note   1. The Generic Location Setup Server model extends the Generic Location
 *             Server model.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_location_setup_srv_t.
 *
 *  @return New Generic Location Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_LOCATION_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_USER_PROP_SRV
 *
 *  @brief  Define a new Generic User Property Server Model.
 *
 *  @note   1. The Generic User Property Server model is a root model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_user_prop_srv_t.
 *
 *  @return New Generic User Property Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_USER_PROP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_ADMIN_PROP_SRV
 *
 *  @brief  Define a new Generic Admin Property Server Model.
 *
 *  @note   1. The Generic Admin Property Server model extends the Generic User
 *             Property Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_admin_prop_srv_t.
 *
 *  @return New Generic Admin Property Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_ADMIN_PROP_SRV(srv_pub, srv_data)            \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV,    \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_MANUFACTURER_PROP_SRV
 *
 *  @brief  Define a new Generic Manufacturer Property Server Model.
 *
 *  @note   1. The Generic Manufacturer Property Server model extends the Generic
 *             User Property Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_manu_prop_srv_t.
 *
 *  @return New Generic Manufacturer Property Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_MANUFACTURER_PROP_SRV(srv_pub, srv_data)            \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV,    \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_GEN_CLIENT_PROP_SRV
 *
 *  @brief  Define a new Generic User Property Server Model.
 *
 *  @note   1. The Generic Client Property Server model is a root model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_gen_client_prop_srv_t.
 *
 *  @return New Generic Client Property Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_GEN_CLIENT_PROP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV,     \
                    NULL, srv_pub, srv_data)

/** Parameters of Generic OnOff state */
typedef struct {
    uint8_t onoff;          /*!< The present value of the Generic OnOff state */
    uint8_t target_onoff;   /*!< The target value of the Generic OnOff state */
} esp_ble_mesh_gen_onoff_state_t;

/** User data of Generic OnOff Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic OnOff Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_onoff_state_t state;       /*!< Parameters of the Generic OnOff state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
} esp_ble_mesh_gen_onoff_srv_t;

/** Parameters of Generic Level state */
typedef struct {
    int16_t level;          /*!< The present value of the Generic Level state */
    int16_t target_level;   /*!< The target value of the Generic Level state */

    /**
     * When a new transaction starts, level should be set to last_last, and use
     * "level + incoming delta" to calculate the target level. In another word,
     * "last_level" is used to record "level" of the last transaction, and
     * "last_delta" is used to record the previously received delta_level value.
     */
    int16_t last_level; /*!< The last value of the Generic Level state */
    int32_t last_delta; /*!< The last delta change of the Generic Level state */

    bool move_start;    /*!< Indicate if the transition of the Generic Level state has been started */
    bool positive;      /*!< Indicate if the transition is positive or negative */
} esp_ble_mesh_gen_level_state_t;

/** User data of Generic Level Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Level Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_level_state_t state;       /*!< Parameters of the Generic Level state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_level;                     /*!< Delta change value of level state transition */
} esp_ble_mesh_gen_level_srv_t;

/** Parameter of Generic Default Transition Time state */
typedef struct {
    uint8_t trans_time;     /*!< The value of the Generic Default Transition Time state */
} esp_ble_mesh_gen_def_trans_time_state_t;

/** User data of Generic Default Transition Time Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic Default Transition Time Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_def_trans_time_state_t state;  /*!< Parameters of the Generic Default Transition Time state */
} esp_ble_mesh_gen_def_trans_time_srv_t;

/** Parameter of Generic OnPowerUp state */
typedef struct {
    uint8_t onpowerup;      /*!< The value of the Generic OnPowerUp state */
} esp_ble_mesh_gen_onpowerup_state_t;

/** User data of Generic Power OnOff Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Power OnOff Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_onpowerup_state_t *state;  /*!< Parameters of the Generic OnPowerUp state */
} esp_ble_mesh_gen_power_onoff_srv_t;

/** User data of Generic Power OnOff Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Power OnOff Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_onpowerup_state_t *state;  /*!< Parameters of the Generic OnPowerUp state */
} esp_ble_mesh_gen_power_onoff_setup_srv_t;

/** Parameters of Generic Power Level state */
typedef struct {
    uint16_t power_actual;          /*!< The present value of the Generic Power Actual state */
    uint16_t target_power_actual;   /*!< The target value of the Generic Power Actual state */

    uint16_t power_last;            /*!< The value of the Generic Power Last state */
    uint16_t power_default;         /*!< The value of the Generic Power Default state */

    uint8_t  status_code;           /*!< The status code of setting Generic Power Range state */
    uint16_t power_range_min;       /*!< The minimum value of the Generic Power Range state */
    uint16_t power_range_max;       /*!< The maximum value of the Generic Power Range state */
} esp_ble_mesh_gen_power_level_state_t;

/** User data of Generic Power Level Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic Power Level Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_power_level_state_t *state;    /*!< Parameters of the Generic Power Level state */
    esp_ble_mesh_last_msg_info_t last;              /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition;     /*!< Parameters of state transition */
    int32_t tt_delta_level;                         /*!< Delta change value of level state transition */
} esp_ble_mesh_gen_power_level_srv_t;

/** User data of Generic Power Level Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic Power Level Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_power_level_state_t *state;    /*!< Parameters of the Generic Power Level state */
} esp_ble_mesh_gen_power_level_setup_srv_t;

/** Parameters of Generic Battery state */
typedef struct {
    uint32_t battery_level : 8,         /*!< The value of the Generic Battery Level state */
             time_to_discharge : 24;    /*!< The value of the Generic Battery Time to Discharge state */
    uint32_t time_to_charge : 24,       /*!< The value of the Generic Battery Time to Charge state */
             battery_flags : 8;         /*!< The value of the Generic Battery Flags state */
} esp_ble_mesh_gen_battery_state_t;

/** User data of Generic Battery Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Battery Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_battery_state_t state;     /*!< Parameters of the Generic Battery state */
} esp_ble_mesh_gen_battery_srv_t;

/** Parameters of Generic Location state */
typedef struct {
    int32_t  global_latitude;   /*!< The value of the Global Latitude field */
    int32_t  global_longitude;  /*!< The value of the Global Longitude field */
    int16_t  global_altitude;   /*!< The value of the Global Altitude field */
    int16_t  local_north;       /*!< The value of the Local North field */
    int16_t  local_east;        /*!< The value of the Local East field */
    int16_t  local_altitude;    /*!< The value of the Local Altitude field */
    uint8_t  floor_number;      /*!< The value of the Floor Number field */
    uint16_t uncertainty;       /*!< The value of the Uncertainty field */
} esp_ble_mesh_gen_location_state_t;

/** User data of Generic Location Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Location Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_location_state_t *state;   /*!< Parameters of the Generic Location state */
} esp_ble_mesh_gen_location_srv_t;

/** User data of Generic Location Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Location Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_gen_location_state_t *state;   /*!< Parameters of the Generic Location state */
} esp_ble_mesh_gen_location_setup_srv_t;

/** This enum value is the access value of Generic User Property */
typedef enum {
    ESP_BLE_MESH_GEN_USER_ACCESS_PROHIBIT,
    ESP_BLE_MESH_GEN_USER_ACCESS_READ,
    ESP_BLE_MESH_GEN_USER_ACCESS_WRITE,
    ESP_BLE_MESH_GEN_USER_ACCESS_READ_WRITE,
} esp_ble_mesh_gen_user_prop_access_t;

/** This enum value is the access value of Generic Admin Property */
typedef enum {
    ESP_BLE_MESH_GEN_ADMIN_NOT_USER_PROP,
    ESP_BLE_MESH_GEN_ADMIN_ACCESS_READ,
    ESP_BLE_MESH_GEN_ADMIN_ACCESS_WRITE,
    ESP_BLE_MESH_GEN_ADMIN_ACCESS_READ_WRITE,
} esp_ble_mesh_gen_admin_prop_access_t;

/** This enum value is the access value of Generic Manufacturer Property */
typedef enum {
    ESP_BLE_MESH_GEN_MANU_NOT_USER_PROP,
    ESP_BLE_MESH_GEN_MANU_ACCESS_READ,
} esp_ble_mesh_gen_manu_prop_access_t;

/** Parameters of Generic Property states */
typedef struct {
    uint16_t id;            /*!< The value of User/Admin/Manufacturer Property ID */
    uint8_t  user_access;   /*!< The value of User Access field */
    uint8_t  admin_access;  /*!< The value of Admin Access field */
    uint8_t  manu_access;   /*!< The value of Manufacturer Access field */
    struct net_buf_simple *val; /*!< The value of User/Admin/Manufacturer Property */
} esp_ble_mesh_generic_property_t;

/** User data of Generic User Property Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic User Property Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    uint8_t property_count;                         /*!< Generic User Property count */
    esp_ble_mesh_generic_property_t *properties;    /*!< Parameters of the Generic User Property state */
} esp_ble_mesh_gen_user_prop_srv_t;

/** User data of Generic Admin Property Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic Admin Property Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    uint8_t property_count;                         /*!< Generic Admin Property count */
    esp_ble_mesh_generic_property_t *properties;    /*!< Parameters of the Generic Admin Property state */
} esp_ble_mesh_gen_admin_prop_srv_t;

/** User data of Generic Manufacturer Property Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Generic Manufacturer Property Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    uint8_t property_count;                         /*!< Generic Manufacturer Property count */
    esp_ble_mesh_generic_property_t *properties;    /*!< Parameters of the Generic Manufacturer Property state */
} esp_ble_mesh_gen_manu_prop_srv_t;

/** User data of Generic Client Property Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Generic Client Property Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    uint8_t id_count;                           /*!< Generic Client Property ID count */
    uint16_t *property_ids;                     /*!< Parameters of the Generic Client Property state */
} esp_ble_mesh_gen_client_prop_srv_t;

/** Parameter of Generic OnOff Set state change event */
typedef struct {
    uint8_t onoff;          /*!< The value of Generic OnOff state */
} esp_ble_mesh_state_change_gen_onoff_set_t;

/** Parameter of Generic Level Set state change event */
typedef struct {
    int16_t level;          /*!< The value of Generic Level state */
} esp_ble_mesh_state_change_gen_level_set_t;

/** Parameter of Generic Delta Set state change event */
typedef struct {
    int16_t level;          /*!< The value of Generic Level state */
} esp_ble_mesh_state_change_gen_delta_set_t;

/** Parameter of Generic Move Set state change event */
typedef struct {
    int16_t level;          /*!< The value of Generic Level state */
} esp_ble_mesh_state_change_gen_move_set_t;

/** Parameter of Generic Default Transition Time Set state change event */
typedef struct {
    uint8_t trans_time;     /*!< The value of Generic Default Transition Time state */
} esp_ble_mesh_state_change_gen_def_trans_time_set_t;

/** Parameter of Generic OnPowerUp Set state change event */
typedef struct {
    uint8_t onpowerup;      /*!< The value of Generic OnPowerUp state */
} esp_ble_mesh_state_change_gen_onpowerup_set_t;

/** Parameter of Generic Power Level Set state change event */
typedef struct {
    uint16_t power;         /*!< The value of Generic Power Actual state */
} esp_ble_mesh_state_change_gen_power_level_set_t;

/** Parameter of Generic Power Default Set state change event */
typedef struct {
    uint16_t power;         /*!< The value of Generic Power Default state */
} esp_ble_mesh_state_change_gen_power_default_set_t;

/** Parameters of Generic Power Range Set state change event */
typedef struct {
    uint16_t range_min;     /*!< The minimum value of Generic Power Range state */
    uint16_t range_max;     /*!< The maximum value of Generic Power Range state */
} esp_ble_mesh_state_change_gen_power_range_set_t;

/** Parameters of Generic Location Global Set state change event */
typedef struct {
    int32_t latitude;       /*!< The Global Latitude value of Generic Location state */
    int32_t longitude;      /*!< The Global Longitude value of Generic Location state */
    int16_t altitude;       /*!< The Global Altitude value of Generic Location state */
} esp_ble_mesh_state_change_gen_loc_global_set_t;

/** Parameters of Generic Location Local Set state change event */
typedef struct {
    int16_t  north;         /*!< The Local North value of Generic Location state */
    int16_t  east;          /*!< The Local East value of Generic Location state */
    int16_t  altitude;      /*!< The Local Altitude value of Generic Location state */
    uint8_t  floor_number;  /*!< The Floor Number value of Generic Location state */
    uint16_t uncertainty;   /*!< The Uncertainty value of Generic Location state */
} esp_ble_mesh_state_change_gen_loc_local_set_t;

/** Parameters of Generic User Property Set state change event */
typedef struct {
    uint16_t id;    /*!< The property id of Generic User Property state */
    struct net_buf_simple *value;   /*!< The property value of Generic User Property state */
} esp_ble_mesh_state_change_gen_user_property_set_t;

/** Parameters of Generic Admin Property Set state change event */
typedef struct {
    uint16_t id;        /*!< The property id of Generic Admin Property state */
    uint8_t  access;    /*!< The property access of Generic Admin Property state */
    struct net_buf_simple *value;   /*!< The property value of Generic Admin Property state */
} esp_ble_mesh_state_change_gen_admin_property_set_t;

/** Parameters of Generic Manufacturer Property Set state change event */
typedef struct {
    uint16_t id;        /*!< The property id of Generic Manufacturer Property state */
    uint8_t  access;    /*!< The property value of Generic Manufacturer Property state */
} esp_ble_mesh_state_change_gen_manu_property_set_t;

/**
 * @brief Generic Server Model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_gen_onoff_set_t          onoff_set;           /*!< Generic OnOff Set */
    esp_ble_mesh_state_change_gen_level_set_t          level_set;           /*!< Generic Level Set */
    esp_ble_mesh_state_change_gen_delta_set_t          delta_set;           /*!< Generic Delta Set */
    esp_ble_mesh_state_change_gen_move_set_t           move_set;            /*!< Generic Move Set */
    esp_ble_mesh_state_change_gen_def_trans_time_set_t def_trans_time_set;  /*!< Generic Default Transition Time Set */
    esp_ble_mesh_state_change_gen_onpowerup_set_t      onpowerup_set;       /*!< Generic OnPowerUp Set */
    esp_ble_mesh_state_change_gen_power_level_set_t    power_level_set;     /*!< Generic Power Level Set */
    esp_ble_mesh_state_change_gen_power_default_set_t  power_default_set;   /*!< Generic Power Default Set */
    esp_ble_mesh_state_change_gen_power_range_set_t    power_range_set;     /*!< Generic Power Range Set */
    esp_ble_mesh_state_change_gen_loc_global_set_t     loc_global_set;      /*!< Generic Location Global Set */
    esp_ble_mesh_state_change_gen_loc_local_set_t      loc_local_set;       /*!< Generic Location Local Set */
    esp_ble_mesh_state_change_gen_user_property_set_t  user_property_set;   /*!< Generic User Property Set */
    esp_ble_mesh_state_change_gen_admin_property_set_t admin_property_set;  /*!< Generic Admin Property Set */
    esp_ble_mesh_state_change_gen_manu_property_set_t  manu_property_set;   /*!< Generic Manufacturer Property Set */
} esp_ble_mesh_generic_server_state_change_t;

/** Context of the received Generic User Property Get message */
typedef struct {
    uint16_t property_id;       /*!< Property ID identifying a Generic User Property */
} esp_ble_mesh_server_recv_gen_user_property_get_t;

/** Context of the received Generic Admin Property Get message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Admin Property */
} esp_ble_mesh_server_recv_gen_admin_property_get_t;

/** Context of the received Generic Manufacturer Property message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Manufacturer Property */
} esp_ble_mesh_server_recv_gen_manufacturer_property_get_t;

/** Context of the received Generic Client Properties Get message */
typedef struct {
    uint16_t property_id;   /*!< A starting Client Property ID present within an element */
} esp_ble_mesh_server_recv_gen_client_properties_get_t;

/**
 * @brief Generic Server Model received get message union
 */
typedef union {
    esp_ble_mesh_server_recv_gen_user_property_get_t         user_property;     /*!< Generic User Property Get */
    esp_ble_mesh_server_recv_gen_admin_property_get_t        admin_property;    /*!< Generic Admin Property Get */
    esp_ble_mesh_server_recv_gen_manufacturer_property_get_t manu_property;     /*!< Generic Manufacturer Property Get */
    esp_ble_mesh_server_recv_gen_client_properties_get_t     client_properties; /*!< Generic Client Properties Get */
} esp_ble_mesh_generic_server_recv_get_msg_t;

/** Context of the received Generic OnOff Set message */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    uint8_t onoff;          /*!< Target value of Generic OnOff state */
    uint8_t tid;            /*!< Transaction ID */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_gen_onoff_set_t;

/** Context of the received Generic Level Set message */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    int16_t level;          /*!< Target value of Generic Level state */
    uint8_t tid;            /*!< Transaction ID */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_gen_level_set_t;

/** Context of the received Generic Delta Set message */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    int32_t delta_level;    /*!< Delta change of Generic Level state */
    uint8_t tid;            /*!< Transaction ID */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_gen_delta_set_t;

/** Context of the received Generic Move Set message */
typedef struct {
    bool    op_en;          /*!< Indicate if optional parameters are included */
    int16_t delta_level;    /*!< Delta Level step to calculate Move speed for Generic Level state */
    uint8_t tid;            /*!< Transaction ID */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_gen_move_set_t;

/** Context of the received Generic Default Transition Time Set message */
typedef struct {
    uint8_t trans_time;     /*!< The value of the Generic Default Transition Time state */
} esp_ble_mesh_server_recv_gen_def_trans_time_set_t;

/** Context of the received Generic OnPowerUp Set message */
typedef struct {
    uint8_t onpowerup;      /*!< The value of the Generic OnPowerUp state */
} esp_ble_mesh_server_recv_gen_onpowerup_set_t;

/** Context of the received Generic Power Level Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t power;         /*!< Target value of Generic Power Actual state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_gen_power_level_set_t;

/** Context of the received Generic Power Default Set message */
typedef struct {
    uint16_t power;         /*!< The value of the Generic Power Default state */
} esp_ble_mesh_server_recv_gen_power_default_set_t;

/** Context of the received Generic Power Range Set message */
typedef struct {
    uint16_t range_min;     /*!< Value of Range Min field of Generic Power Range state */
    uint16_t range_max;     /*!< Value of Range Max field of Generic Power Range state */
} esp_ble_mesh_server_recv_gen_power_range_set_t;

/** Context of the received Generic Location Global Set message */
typedef struct {
    int32_t global_latitude;    /*!< Global Coordinates (Latitude) */
    int32_t global_longitude;   /*!< Global Coordinates (Longitude) */
    int16_t global_altitude;    /*!< Global Altitude */
} esp_ble_mesh_server_recv_gen_loc_global_set_t;

/** Context of the received Generic Location Local Set message */
typedef struct {
    int16_t  local_north;       /*!< Local Coordinates (North) */
    int16_t  local_east;        /*!< Local Coordinates (East) */
    int16_t  local_altitude;    /*!< Local Altitude */
    uint8_t  floor_number;      /*!< Floor Number */
    uint16_t uncertainty;       /*!< Uncertainty */
} esp_ble_mesh_server_recv_gen_loc_local_set_t;

/** Context of the received Generic User Property Set message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic User Property */
    struct net_buf_simple *property_value;  /*!< Raw value for the User Property */
} esp_ble_mesh_server_recv_gen_user_property_set_t;

/** Context of the received Generic Admin Property Set message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Admin Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access */
    struct net_buf_simple *property_value;  /*!< Raw value for the Admin Property */
} esp_ble_mesh_server_recv_gen_admin_property_set_t;

/** Context of the received Generic Manufacturer Property Set message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Generic Manufacturer Property */
    uint8_t  user_access;   /*!< Enumeration indicating user access */
} esp_ble_mesh_server_recv_gen_manufacturer_property_set_t;

/**
 * @brief Generic Server Model received set message union
 */
typedef union {
    esp_ble_mesh_server_recv_gen_onoff_set_t                 onoff;             /*!< Generic OnOff Set/Generic OnOff Set Unack */
    esp_ble_mesh_server_recv_gen_level_set_t                 level;             /*!< Generic Level Set/Generic Level Set Unack */
    esp_ble_mesh_server_recv_gen_delta_set_t                 delta;             /*!< Generic Delta Set/Generic Delta Set Unack */
    esp_ble_mesh_server_recv_gen_move_set_t                  move;              /*!< Generic Move Set/Generic Move Set Unack */
    esp_ble_mesh_server_recv_gen_def_trans_time_set_t        def_trans_time;    /*!< Generic Default Transition Time Set/Generic Default Transition Time Set Unack */
    esp_ble_mesh_server_recv_gen_onpowerup_set_t             onpowerup;         /*!< Generic OnPowerUp Set/Generic OnPowerUp Set Unack */
    esp_ble_mesh_server_recv_gen_power_level_set_t           power_level;       /*!< Generic Power Level Set/Generic Power Level Set Unack */
    esp_ble_mesh_server_recv_gen_power_default_set_t         power_default;     /*!< Generic Power Default Set/Generic Power Default Set Unack */
    esp_ble_mesh_server_recv_gen_power_range_set_t           power_range;       /*!< Generic Power Range Set/Generic Power Range Set Unack */
    esp_ble_mesh_server_recv_gen_loc_global_set_t            location_global;   /*!< Generic Location Global Set/Generic Location Global Set Unack */
    esp_ble_mesh_server_recv_gen_loc_local_set_t             location_local;    /*!< Generic Location Local Set/Generic Location Local Set Unack */
    esp_ble_mesh_server_recv_gen_user_property_set_t         user_property;     /*!< Generic User Property Set/Generic User Property Set Unack */
    esp_ble_mesh_server_recv_gen_admin_property_set_t        admin_property;    /*!< Generic Admin Property Set/Generic Admin Property Set Unack */
    esp_ble_mesh_server_recv_gen_manufacturer_property_set_t manu_property;     /*!< Generic Manufacturer Property Set/Generic Manufacturer Property Set Unack */
} esp_ble_mesh_generic_server_recv_set_msg_t;

/**
 * @brief Generic Server Model callback value union
 */
typedef union {
    esp_ble_mesh_generic_server_state_change_t state_change;    /*!< ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT */
    esp_ble_mesh_generic_server_recv_get_msg_t get;     /*!< ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT */
    esp_ble_mesh_generic_server_recv_set_msg_t set;     /*!< ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT */
} esp_ble_mesh_generic_server_cb_value_t;

/** Generic Server Model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;   /*!< Pointer to Generic Server Models */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received messages */
    esp_ble_mesh_generic_server_cb_value_t value;   /*!< Value of the received Generic Messages */
} esp_ble_mesh_generic_server_cb_param_t;

/** This enum value is the event of Generic Server Model */
typedef enum {
    /**
     * 1. When get_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, no event will be
     *    callback to the application layer when Generic Get messages are received.
     * 2. When set_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, this event will
     *    be callback to the application layer when Generic Set/Set Unack messages
     *    are received.
     */
    ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT,
    /**
     * When get_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Generic Get messages are received.
     */
    ESP_BLE_MESH_GENERIC_SERVER_RECV_GET_MSG_EVT,
    /**
     * When set_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Generic Set/Set Unack messages are received.
     */
    ESP_BLE_MESH_GENERIC_SERVER_RECV_SET_MSG_EVT,
    ESP_BLE_MESH_GENERIC_SERVER_EVT_MAX,
} esp_ble_mesh_generic_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Generic Server Model function.
 */

/**
 * @brief   Generic Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_generic_server_cb_t)(esp_ble_mesh_generic_server_cb_event_t event,
                                                  esp_ble_mesh_generic_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Generic Server Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_generic_server_callback(esp_ble_mesh_generic_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_GENERIC_MODEL_API_H_ */
