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
 *  @brief Bluetooth Mesh Sensor Client Model APIs.
 */

#ifndef _SENSOR_CLIENT_H_
#define _SENSOR_CLIENT_H_

#include "mesh_access.h"
#include "mesh_kernel.h"

#include "client_common.h"

/* Sensor Client Model Context */
extern const struct bt_mesh_model_op sensor_cli_op[];

/** @def BLE_MESH_MODEL_SENSOR_CLI
 *
 *  Define a new sensor client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a sensor client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_sensor_cli.
 *
 *  @return New sensor client model instance.
 */
#define BLE_MESH_MODEL_SENSOR_CLI(cli_pub, cli_data)    \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_SENSOR_CLI,    \
                    sensor_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t      bt_mesh_sensor_client_t;
typedef bt_mesh_client_internal_data_t  sensor_internal_data_t;

struct bt_mesh_sensor_descriptor_status {
    struct net_buf_simple *descriptor; /* Sequence of 8-octet sensor descriptors (optional) */
};

struct bt_mesh_sensor_cadence_status {
    u16_t property_id;              /* Property for the sensor                       */
    struct net_buf_simple *sensor_cadence_value; /* Value of sensor cadence state */
};

struct bt_mesh_sensor_settings_status {
    u16_t sensor_property_id;              /* Property ID identifying a sensor                        */
    struct net_buf_simple *sensor_setting_property_ids; /* A sequence of N sensor setting property IDs (optional) */
};

struct bt_mesh_sensor_setting_status {
    bool  op_en;                      /* Indicate whether optional parameters included       */
    u16_t sensor_property_id;         /* Property ID identifying a sensor                    */
    u16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor    */
    u8_t  sensor_setting_access;      /* Read/Write access rights for the setting (optional) */
    struct net_buf_simple *sensor_setting_raw; /* Raw value for the setting */
};

struct bt_mesh_sensor_status {
    struct net_buf_simple *marshalled_sensor_data; /* Value of sensor data state (optional) */
};

struct bt_mesh_sensor_column_status {
    u16_t property_id;             /* Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_column_value; /* Left values of sensor column status */
};

struct bt_mesh_sensor_series_status {
    u16_t property_id;             /* Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_series_value; /* Left values of sensor series status */
};

struct bt_mesh_sensor_descriptor_get {
    bool  op_en;       /* Indicate whether optional parameters included */
    u16_t property_id; /* Property ID for the sensor (optional)         */
};

struct bt_mesh_sensor_cadence_get {
    u16_t property_id; /* Property ID for the sensor */
};

struct bt_mesh_sensor_cadence_set {
    u16_t property_id;                     /* Property ID for the sensor                                */
    u8_t  fast_cadence_period_divisor : 7, /* Divisor for the publish period                            */
          status_trigger_type : 1;         /* The unit and format of the Status Trigger Delta fields    */
    struct net_buf_simple *status_trigger_delta_down; /* Delta down value that triggers a status message */
    struct net_buf_simple *status_trigger_delta_up; /* Delta up value that triggers a status message */
    u8_t  status_min_interval;             /* Minimum interval between two consecutive Status messages  */
    struct net_buf_simple *fast_cadence_low; /* Low value for the fast cadence range */
    struct net_buf_simple *fast_cadence_high; /* Fast value for the fast cadence range */
};

struct bt_mesh_sensor_settings_get {
    u16_t sensor_property_id; /* Property ID for the sensor */
};

struct bt_mesh_sensor_setting_get {
    u16_t sensor_property_id;         /* Property ID identifying a sensor                 */
    u16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor */
};

struct bt_mesh_sensor_setting_set {
    u16_t sensor_property_id;         /* Property ID identifying a sensor                 */
    u16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor */
    struct net_buf_simple *sensor_setting_raw; /* Raw value for the setting */
};

struct bt_mesh_sensor_get {
    bool  op_en;       /* Indicate whether optional parameters included */
    u16_t property_id; /* Property ID for the sensor (optional)         */
};

struct bt_mesh_sensor_column_get {
    u16_t property_id;     /* Property identifying a sensor                */
    struct net_buf_simple *raw_value_x; /* Raw value identifying a column */
};

struct bt_mesh_sensor_series_get {
    bool  op_en;            /* Indicate whether optional parameters included         */
    u16_t property_id;      /* Property identifying a sensor                         */
    struct net_buf_simple *raw_value_x1; /* Raw value identifying a starting column (optional) */
    struct net_buf_simple *raw_value_x2; /* Raw value identifying a ending column (C.1) */
};

/**
 * @brief This function is called to initialize sensor client model user_data.
 *
 * @param[in] model:   Pointer to sensor client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_sensor_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to get sensor states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of sensor get message value
 * @param[out] status: Pointer of sensor status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_sensor_client_get_state(bt_mesh_client_common_param_t *common, void *get, void *status);

/**
 * @brief This function is called to set sensor states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of sensor set message value
 * @param[out] status: Pointer of sensor status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_sensor_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status);

#endif /* _SENSOR_CLIENT_H_ */
